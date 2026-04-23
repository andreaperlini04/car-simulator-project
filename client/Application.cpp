#include "Application.h"
#include "light.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

// Bridge C-style per mappare le callback dell'engine sull'istanza corrente
static Application* s_app = nullptr;

static void s_displayCallback() { s_app->onDisplay(); }
static void s_keyboardCallback(unsigned char k, int x, int y) { s_app->onKeyboard(k, x, y); }
static void s_keyboardUpCallback(unsigned char k) { s_app->onKeyboardUp(k); }
static void s_specialCallback(int k, int x, int y) { s_app->onSpecial(k, x, y); }
static void s_reshapeCallback(int w, int h) { s_app->onReshape(w, h); }
static void s_mouseMotionCallback(int x, int y) { s_app->onMouseMotion(x, y); }

Application::Application()
   : engine(Eng::Base::getInstance())
   , myCar(GAME_MAX_SPEED, GAME_ACCEL, GAME_BRAKE, GAME_FRICTION, GAME_REVERSE_MAX)
{
   s_app = this;
}

bool Application::init(int argc, char* argv[])
{
   if (!engine.init(argc, argv)) return false;

   engine.createWindow(800, 600, 100, 100, "Car Simulator");
   engine.enableFPS();

   engine.setDisplayCallback(s_displayCallback);
   engine.setKeyboardCallback(s_keyboardCallback);
   engine.setKeyboardUpCallback(s_keyboardUpCallback);
   engine.setSpecialCallback(s_specialCallback);
   engine.setReshapeCallback(s_reshapeCallback);
   engine.setMouseMotionCallback(s_mouseMotionCallback);

   return loadScene();
}

void Application::run()
{
   engine.update();
   engine.free();
}

bool Application::loadScene()
{
   auto mainCam = std::make_unique<PerspectiveCamera>(
      "MainCam", 45.0f, 800.0f / 600.0f, 1.0f, 5000.0f);
   camera = mainCam.get();

   list = std::make_unique<List>();

   Node* scena = ovoreader.readFile("macchina.ovo", "texture/");
   if (!scena) {
      std::cerr << "Errore critico: impossibile caricare macchina.ovo\n";
      return false;
   }

   root = std::unique_ptr<Node>(scena);
   // Passaggio di ownership: root gestira' il ciclo di vita della camera
   root->addChild(mainCam.release());

   printSceneGraphWithPosition(root.get());

   Node* carNode = root->findByName("Car");
   if (carNode) {
      myCar.init(carNode);
   }
   else {
      std::cerr << "Attenzione: nodo 'Car' non trovato nello scene graph.\n";
   }

   Node* planeNode = root->findByName("Plane");
   if (planeNode) groundMesh = dynamic_cast<Mesh*>(planeNode);

   reparentLightsToCar();

   printSceneGraphWithPosition(root.get());
   return true;
}

void Application::reparentLightsToCar()
{
   // Fissa le luci della scena al nodo vettura per evitare overhead su calcoli di illuminazione dinamica
   Node* carNode = root->findByName("Car");
   if (!carNode) return;

   const std::string lightNames[] = {
       "Omni", "Omni001", "Omni002", "Omni003", "Omni004"
   };

   for (const std::string& name : lightNames) {
      Node* light = root->findByName(name);
      if (light && light->getParent()) {
         auto extracted = light->getParent()->removeChild(light);
         carNode->addChild(std::move(extracted));
      }
   }
}

/**
 * @brief Main loop fisico e di rendering.
 * Implementa un fixed-timestep per la fisica, disaccoppiando
 * le collisioni e le forze dal framerate visivo per garantire determinismo.
 */
void Application::onDisplay()
{
   auto now = std::chrono::steady_clock::now();
   if (isFirstFrame) {
      lastFrameTime = now;
      isFirstFrame = false;
   }

   double frameTime = std::chrono::duration<double>(now - lastFrameTime).count();
   lastFrameTime = now;

   // Spiral-of-death guard: limita l'accumulo di dt causato da lag o breakpoint
   if (frameTime > PHYSICS_MAX_FRAME)
      frameTime = PHYSICS_MAX_FRAME;

   physicsAccumulator += frameTime;
   while (physicsAccumulator >= PHYSICS_FIXED_DT) {
      myCar.stepPhysics(PHYSICS_FIXED_DT);
      physicsAccumulator -= PHYSICS_FIXED_DT;
   }

   // L'interpolazione grafica (es. rotazione mesh ruote) usa il dt reale
   myCar.updateRenderer(frameTime);

   updateCameraFollow(selectedCamera.current);

   list->clear();
   list->pass(root.get(), glm::mat4(1.0f));

   printCustomText();

   engine.setRenderList(list.get());
   engine.setMainCamera(camera);
   engine.postRedisplay();
}

void Application::onKeyboard(unsigned char key, int /*x*/, int /*y*/)
{
   switch (key) {
   case 'E': case 'e':
      if (!isGameStarted) isGameStarted = true;
      myCar.startEngine();
      break;

   case 'T': case 't':
      myCar.turnOffEngine();
      break;

   case 'w': myCar.setAccelerating(true); break;
   case 's': myCar.setBraking(true);      break;
   case ' ': myCar.setHandbrake(true);    break;

   case 'm':
      orbit.isMotionCameraActivated = !orbit.isMotionCameraActivated;
      if (orbit.isMotionCameraActivated) {
         engine.setCursorVisible(false);
         engine.warpMouse(engine.getWindowWidth() / 2, engine.getWindowHeight() / 2);
      }
      else if (!mouseSteering.isActive) {
         engine.setCursorVisible(true);
      }
      break;

   case 'n': case 'N':
      // Attivazione mouse steering, anche se non è bellissimo da vedere e usare
      mouseSteering.isActive = !mouseSteering.isActive;
      if (mouseSteering.isActive) {
         myCar.setMouseSteering(true);
         myCar.setSteeringLeft(false);
         myCar.setSteeringRight(false);
         myCar.setMouseSteeringTarget(0.0);
         engine.setCursorVisible(false);
         ignoreNextMotion = true;
         engine.warpMouse(engine.getWindowWidth() / 2, engine.getWindowHeight() / 2);
         mouseSteering.currentAngle = 0.0f;
      }
      else {
         myCar.setMouseSteering(false);
         if (!orbit.isMotionCameraActivated) engine.setCursorVisible(true);
         mouseSteering.currentAngle = 0.0f;
      }
      break;

   case 'a':
      if (isGameStarted && !mouseSteering.isActive) myCar.setSteeringLeft(true);
      break;
   case 'd':
      if (isGameStarted && !mouseSteering.isActive) myCar.setSteeringRight(true);
      break;

   case 'u':
      if (groundMesh) groundMesh->setWireframe(!groundMesh->getWireframe());
      break;

   case '1': selectedCamera.current = CameraSelection::BEHIND; break;
   case '2': selectedCamera.current = CameraSelection::LEFT;   break;
   case '3': selectedCamera.current = CameraSelection::RIGHT;  break;
   }

   engine.postRedisplay();
}

void Application::onKeyboardUp(unsigned char key)
{
   switch (key) {
   case 'w': myCar.setAccelerating(false); break;
   case 's': myCar.setBraking(false);      break;
   case ' ': myCar.setHandbrake(false);    break;
   case 'a':
      if (!mouseSteering.isActive) myCar.setSteeringLeft(false);
      break;
   case 'd':
      if (!mouseSteering.isActive) myCar.setSteeringRight(false);
      break;
   }
   engine.postRedisplay();
}

void Application::onSpecial(int /*key*/, int /*x*/, int /*y*/) {}

void Application::onReshape(int width, int height)
{
   if (height == 0) height = 1;
   if (auto pCam = dynamic_cast<PerspectiveCamera*>(camera)) {
      pCam->setAspectRatio(static_cast<float>(width), static_cast<float>(height));
   }
}

void Application::onMouseMotion(int x, int y)
{
   if (!orbit.isMotionCameraActivated && !mouseSteering.isActive) return;

   if (ignoreNextMotion) {
      ignoreNextMotion = false;
      return;
   }

   const int cx = engine.getWindowWidth() / 2;
   const int cy = engine.getWindowHeight() / 2;
   const int deltaX = x - cx;
   const int deltaY = y - cy;

   if (orbit.isMotionCameraActivated && selectedCamera.current == CameraSelection::BEHIND) {
      orbit.yaw -= deltaX * orbit.sensitivity;
      orbit.pitch += deltaY * orbit.sensitivity;

      if (orbit.pitch > 85.0f) orbit.pitch = 85.0f;
      if (orbit.pitch < -2.0f) orbit.pitch = -2.0f;
   }

   if (mouseSteering.isActive) {
      mouseSteering.currentAngle -= deltaX * mouseSteering.sensitivity;
      // Clamp sui limiti hardware/config definiti da CarInputController
      if (mouseSteering.currentAngle > MAX_STEERING_ANGLE_DEG)
         mouseSteering.currentAngle = static_cast<float>(MAX_STEERING_ANGLE_DEG);
      if (mouseSteering.currentAngle < -MAX_STEERING_ANGLE_DEG)
         mouseSteering.currentAngle = -static_cast<float>(MAX_STEERING_ANGLE_DEG);

      myCar.setMouseSteeringTarget(mouseSteering.currentAngle);
   }

   ignoreNextMotion = true;
   engine.warpMouse(cx, cy);
}

void Application::updateCameraFollow(CameraSelection::Position pos)
{
   if (!camera) return;

   // Estrazione del basis vettoriale dalla world matrix per l'orientamento relativo
   const glm::mat4 carMatrix = myCar.getWorldMatrix();
   const glm::vec3 carPosition = glm::vec3(carMatrix[3]);
   const glm::vec3 carRight = glm::normalize(glm::vec3(carMatrix[0]));
   const glm::vec3 carUp = glm::normalize(glm::vec3(carMatrix[1]));
   const glm::vec3 carForward = glm::normalize(glm::vec3(carMatrix[2]));

   glm::vec3 cameraPos, cameraTarget;

   if (pos == CameraSelection::BEHIND) {
      if (orbit.isMotionCameraActivated) {
         const float yawRad = glm::radians(orbit.yaw);
         const float pitchRad = glm::radians(orbit.pitch);

         // Conversione sferica -> cartesiana per offset telecamera orbitale
         const float hDist = orbit.radius * glm::cos(pitchRad);
         const float vDist = orbit.radius * glm::sin(pitchRad);

         glm::vec3 offset = (carForward * glm::cos(yawRad) + carRight * glm::sin(yawRad)) * hDist;
         offset += carUp * vDist;

         cameraPos = carPosition + offset;
         cameraTarget = carPosition + glm::vec3(0.0f, 5.0f, 0.0f);
      }
      else {
         cameraPos = carPosition + (carForward * 100.0f) + (carUp * 100.0f);
         cameraTarget = carPosition + (carUp * 2.0f);
      }
   }
   else if (pos == CameraSelection::LEFT) {
      cameraPos = carPosition - (carRight * 80.0f);
      cameraTarget = carPosition + (carUp * 2.0f);
   }
   else {
      cameraPos = carPosition + (carRight * 80.0f);
      cameraTarget = carPosition + (carUp * 2.0f);
   }

   camera->setM(glm::inverse(glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f))));
}

void Application::drawCenteredText(const std::string& text, float yOffset, float r, float g, float b)
{
   const float x = (engine.getWindowWidth() - engine.getTextWidth(text)) / 2.0f;
   const float y = engine.getWindowHeight() / 2.0f + yOffset;
   engine.addString(x, y, text, r, g, b);
}

std::string Application::getSpeedToDisplay() const
{
   const double kmh = myCar.getCurrSpeedAbs() * METERS_PER_UNIT * 3.6;
   std::ostringstream ss;
   ss << std::fixed << std::setprecision(0) << kmh;
   return "Velocita': " + ss.str() + " km/h";
}

void Application::printCustomText()
{
   if (!isGameStarted) {
      drawCenteredText("Welcome to the Car Simulator", 0.0f, 0.2f, 1.0f, 0.2f);
      drawCenteredText("Press [E] to turn on the engine", -30.0f, 1.0f, 1.0f, 1.0f);
      return;
   }

   engine.clearScreenText();
   engine.addToScreenText("[1] Main Camera");
   engine.addToScreenText("[2-3] Left/Right Camera");
   engine.addToScreenText("[W-S] Accelerate/Brake");
   engine.addToScreenText("[A-D] Turn Left/Right");

   engine.addToScreenText(orbit.isMotionCameraActivated
      ? "[M] Disable Motion Camera [ON]"
      : "[M] Activate Motion Camera [OFF]");

   engine.addToScreenText("\n");

   engine.addToScreenText(mouseSteering.isActive
      ? "[N] Disable Mouse Steering [ON]"
      : "[N] Activate Mouse Steering [OFF]");

   if (myCar.isEngineStarted()) {
      engine.addToScreenText("Car Engine status: ON");
      engine.addToScreenText("[T] Turn off the engine");
   }
   else {
      engine.addToScreenText("Car Engine status: OFF");
      engine.addToScreenText("[E] Turn on the engine");
   }

   if (groundMesh) {
      engine.addToScreenText(groundMesh->getWireframe()
         ? "[u] Disable Wireframe [ON]"
         : "[u] Enable Wireframe [OFF]");
   }

   engine.addToScreenText("[SPACE] Use Handbrake");
   engine.addToScreenText(getSpeedToDisplay());
}

void Application::printSceneGraphWithPosition(Node* node, int level)
{
   if (!node) return;

   const std::string indent(level * 4, ' ');
   const std::string branch = (level == 0) ? "ROOT " : "|__ ";
   const glm::mat4 worldMatrix = node->getWorldFinalMatrix();

   std::cout << indent << branch
      << "ID:" << node->getId()
      << " '" << node->getName() << "'";

   if (dynamic_cast<Mesh*>(node)) std::cout << " [MESH]";
   else if (dynamic_cast<Light*>(node)) std::cout << " [LIGHT]";
   else if (dynamic_cast<Camera*>(node)) std::cout << " [CAMERA]";

   std::cout << std::fixed << std::setprecision(5)
      << " -> Pos: ("
      << worldMatrix[3][0] << ", "
      << worldMatrix[3][1] << ", "
      << worldMatrix[3][2] << ")\n";

   for (unsigned int i = 0; i < node->getNumChildren(); i++) {
      printSceneGraphWithPosition(node->getChild(i), level + 1);
   }
}