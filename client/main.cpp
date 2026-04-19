//////////////
// #INCLUDE //
//////////////

#include "engine.h"
#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "material.h"
#include "omnidirectionalLight.h"
#include "ovoReader.h"
#include "perspectiveCamera.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <optional>
#include <cmath>
#include <array>
#include <iomanip>
#include "Car.h"
#include "WorldConfig.h"

// --- GLOBALI ---
Eng::Base* engine;
Camera* camera;
List* list;
Node* root;
Node* omniLight;
OvoReader ovoreader{};

Car myCar(GAME_MAX_SPEED, GAME_ACCEL, GAME_BRAKE, GAME_FRICTION, GAME_REVERSE_MAX);

bool isGameStarted = false;
Mesh* groundMesh = nullptr;

glm::mat4 mainCameraHome{ 1.0f };

bool ignoreNextMotion = false;

// Timer
auto lastFrameTime = std::chrono::steady_clock::now();
bool isFirstFrame = true;

////////////////
// STRUCT
// Orbital Camera: Mouse movement
///////////////
struct OrbitCameraState {
    float yaw = 0.0f;           // Orizzontal rotation
    float pitch = 30.0f;        // Vertical rotation (inclination)
    float radius = 100.0f;       // Distance from the car 
    float sensitivity = 0.1f;   // Mouse
    
    bool isMotionCameraActivated = false;
};

struct MouseSteeringState {
   bool isActive = false;
   float sensitivity = 0.1f;  // Regola a piacere
   float currentAngle = 0.0f; // Angolo di sterzo corrente
};

MouseSteeringState mouseSteering;

struct CameraSelection {
    enum Position { BEHIND = 1, LEFT = 2, RIGHT = 3 };
    Position current = BEHIND;
};

CameraSelection selectedCamera;  
OrbitCameraState orbit;     // State

// SPEED INFO
double displayKmh;

void updateCameraFollow(CameraSelection::Position pos) {
    if (!camera) return;

    glm::mat4 carMatrix = myCar.getWorldMatrix();
    glm::vec3 carPosition = glm::vec3(carMatrix[3]);

    // Estrazione assi locali della macchina
    glm::vec3 carRight = glm::normalize(glm::vec3(carMatrix[0])); 
    glm::vec3 carUp = glm::normalize(glm::vec3(carMatrix[1])); 
    glm::vec3 carForward = glm::normalize(glm::vec3(carMatrix[2]));

    float distanceBehind = 50.0f;
    float distanceSide = 50.0f; 
    float heightAbove = 50.0f;

    glm::vec3 cameraPos{ 1.f };
    glm::vec3 cameraTarget{ 1.f };
    glm::mat4 viewMatrix{ 1.f };

    if (pos == CameraSelection::BEHIND) { // Camera da DIETRO
        
       if (orbit.isMotionCameraActivated) {    // Orbital Camera

          float yawRad = glm::radians(orbit.yaw);   // Orizzontale
          float pitchRad = glm::radians(orbit.pitch); // Verticale

          // 1. Calcoliamo la distanza orizzontale e verticale in base al pitch
          float hDist = orbit.radius * glm::cos(pitchRad);
          float vDist = orbit.radius * glm::sin(pitchRad);

          // 2. Calcoliamo l'offset RELATIVO alla macchina
          // -carForward * cos(yaw) ci mette esattamente DIETRO la macchina quando yaw è 0
          // carRight * sin(yaw) ci fa orbitare a destra/sinistra quando muoviamo il mouse
          glm::vec3 offset = (carForward * glm::cos(yawRad) + carRight * glm::sin(yawRad)) * hDist;
          offset += carUp * vDist; // Aggiungiamo l'altezza

          cameraPos = carPosition + offset;
          cameraTarget = carPosition + glm::vec3(0.0f, 5.0f, 0.0f);

          viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
        } else {                                // Static Camera that follows the car fro behind
            distanceBehind = 100.0f;
            heightAbove = 100.0f;

            cameraPos = carPosition + (carForward * distanceBehind) + (carUp * heightAbove);
            cameraTarget = carPosition + (carUp * 2.0f);
            viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
        }

    }
    else if (pos == CameraSelection::LEFT) {
        // Camera lato SINISTRO
        distanceSide = 80.0f;
        heightAbove = 0.0f; 

        // Sottraiamo carRight per posizionarci a sinistra della macchina
        cameraPos = carPosition - (carRight * distanceSide) + (carUp * heightAbove);
        cameraTarget = carPosition + (carUp * 2.0f); 

        viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (pos == CameraSelection::RIGHT) {
        //  Camera lato DESTRO
        distanceSide = 80.0f;
        heightAbove = 0.0f;

        // Sommiamo carRight per posizionarci a destra della macchina
        cameraPos = carPosition + (carRight * distanceSide) + (carUp * heightAbove);
        cameraTarget = carPosition + (carUp * 2.0f);

        viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    camera->setM(glm::inverse(viewMatrix));
}

void specialCallback(int key, int x, int y) {
    // Delega la gestione delle frecce alla classe logica
    
}

void printCustomText();

void drawCenteredText(std::string text, float yOffset, float r, float g, float b) {
    int winW = engine->getWindowWidth();
    int winH = engine->getWindowHeight();
    int textWidth = engine->getTextWidth(text);

    float x = (winW - textWidth) / 2.0f;
    float y = (winH / 2.0f) + yOffset;
    engine->addString(x, y, text, r, g, b);
}

void displayCallback() {
   auto currentFrameTime = std::chrono::steady_clock::now();

   if (isFirstFrame) {
       lastFrameTime = currentFrameTime;
       isFirstFrame = false;
   }
   std::chrono::duration<double> elapsedTime = currentFrameTime - lastFrameTime;
   double deltaTime = elapsedTime.count(); //(seconds)
   lastFrameTime = currentFrameTime;

   myCar.update(deltaTime);

   updateCameraFollow(selectedCamera.current);
   list->clear();
   list->pass(root, glm::mat4(1.0f));


   printCustomText();
   

   engine->setRenderList(list);
   engine->setMainCamera(camera);
   engine->postRedisplay();
}

std::string getSpeedToDisplay() {
    // Converti velocità da unità di gioco/s a km/h
    // speedInGameUnits * METERS_PER_UNIT = m/s,  * 3.6 = km/h
    displayKmh = myCar.getCurrSpeedAbs() * METERS_PER_UNIT * 3.6;

    std::ostringstream stream;
    stream << std::fixed << std::setprecision(0) << displayKmh;
    return "Velocita': " + stream.str() + " km/h";
}

void printCustomText() {
   // STARTING TEXT
   if (!isGameStarted) {
      drawCenteredText("Welcome to the Car Simulator", 0.0f, 0.2f, 1.0f, 0.2f); // Verde Lime
      drawCenteredText("Press [E] to turn on the engine", -30.0f, 1.0f, 1.0f, 1.0f); // Bianco
   }
   else { // MENU
      engine->clearScreenText();
      engine->addToScreenText("[1] Main Camera");
      engine->addToScreenText("[2-3] Left/Right Camera");
      engine->addToScreenText("[W-S] Accelerate/Decelerate the car");
      engine->addToScreenText("[A-D] Turn Left/Rigth");
      if (!orbit.isMotionCameraActivated)
         engine->addToScreenText("[M] Activate Motion Camera [OFF]");
      else
         engine->addToScreenText("[M] Disable Motion Camera [ON]");
      engine->addToScreenText("\n");
      if (!mouseSteering.isActive)
         engine->addToScreenText("[N] Activate Mouse Steering [OFF]");
      else
         engine->addToScreenText("[N] Disable Mouse Steering [ON]");

      if (myCar.isEngineStarted()) {
         engine->addToScreenText("Car Engine status: ON");
         engine->addToScreenText("[T] Turn off the engine");
      }
      else {
         engine->addToScreenText("Car Engine status: OFF");
         engine->addToScreenText("[E] Turn on the engine");
      }
      if (groundMesh->getWireframe())
         engine->addToScreenText("[u] Disable Wireframe [ON]");
      else
         engine->addToScreenText("[u] Enable Wireframe [OFF]");
      engine->addToScreenText("[SPACE] Use Handbrake");

      engine-> addToScreenText(getSpeedToDisplay());

      //engine->addToScreenText("[] ");
   }
}



void keyboardCallback(unsigned char key, int x, int y) {

   switch (key) {
   case 'E':
   case 'e':
       if (!isGameStarted)
           isGameStarted = true;
       myCar.startEngine();
       break;
   case 'T':
   case't':
       myCar.turnOffEngine();
       break;

   case 'w': 
      myCar.setAccelerating(true);
      break;
   case 's':
      myCar.setBraking(true);
      break;
   case ' ': // handbrake
      myCar.setHandbrake(true);
      break;
      
   case 'm':
      orbit.isMotionCameraActivated = !orbit.isMotionCameraActivated;
      if (orbit.isMotionCameraActivated) {
         engine->setCursorVisible(false); // Nascondi il cursore

         // Posiziona il mouse al centro per iniziare
         int cx = engine->getWindowWidth() / 2;
         int cy = engine->getWindowHeight() / 2;
         engine->warpMouse(cx, cy);


      }
      else {
          if(!mouseSteering.isActive)
            engine->setCursorVisible(true); // Rimostra il cursore
      }
      break;
   case 'n':
   case 'N':
      mouseSteering.isActive = !mouseSteering.isActive;
      if (mouseSteering.isActive) {
         myCar.setMouseSteering(true);   // attiva la modalità
         myCar.setSteeringLeft(false);
         myCar.setSteeringRight(false);
         myCar.setMouseSteeringTarget(0.0);
         engine->setCursorVisible(false);
         int cx = engine->getWindowWidth() / 2;
         int cy = engine->getWindowHeight() / 2;
         ignoreNextMotion = true;
         engine->warpMouse(cx, cy);
         mouseSteering.currentAngle = 0.0f;
      }
      else {
         myCar.setMouseSteering(false);  // disattiva la modalità
         if(!orbit.isMotionCameraActivated)
            engine->setCursorVisible(true);
         mouseSteering.currentAngle = 0.0f;
      }
      break;
   case 'a':
      if (isGameStarted && !mouseSteering.isActive)
         myCar.setSteeringLeft(true);
      break;
   case 'd':
      if (isGameStarted && !mouseSteering.isActive)
         myCar.setSteeringRight(true);
      break;
   case 'u':
      if (groundMesh) 
         groundMesh->setWireframe(!groundMesh->getWireframe());
      
      break;
   case '1':
       selectedCamera.current = CameraSelection::BEHIND;
       break;
   case '2':
       selectedCamera.current = CameraSelection::LEFT;
       break;
   case '3':
       selectedCamera.current = CameraSelection::RIGHT;

       break;
   }

   // Forza il ridisegno della scena
   engine->postRedisplay();
}

void keyboardUpCallback(unsigned char key) {
   switch (key) {
   case 'w': 
      myCar.setAccelerating(false);
      break;
   case 's': 
      myCar.setBraking(false);
      break;
   case ' ': 
      myCar.setHandbrake(false);
      break;
   case 'a':
      if (!mouseSteering.isActive)
         myCar.setSteeringLeft(false);
      break;
   case 'd':
      if (!mouseSteering.isActive)
         myCar.setSteeringRight(false);
      break;
   }
   
   engine->postRedisplay();
}



void reshapeCallback(int width, int height) {
    if (height == 0) height = 1;
    PerspectiveCamera* pCam = dynamic_cast<PerspectiveCamera*>(camera);
    if (pCam) {
        pCam->setAspectRatio((float)width, (float)height);
    }
}

void mouseMotionCallback(int x, int y) {
   // Nessuna modalità attiva => non fare nulla
   if (!orbit.isMotionCameraActivated && !mouseSteering.isActive) return;

   if (ignoreNextMotion) {          
       ignoreNextMotion = false;
       return;
   }

   int cx = engine->getWindowWidth() / 2;
   int cy = engine->getWindowHeight() / 2;
   int deltaX = x - cx;
   int deltaY = y - cy;

   // --- Camera orbitale (tasto M) ---
   if (orbit.isMotionCameraActivated && selectedCamera.current == CameraSelection::BEHIND) {
      orbit.yaw -= deltaX * orbit.sensitivity;
      orbit.pitch += deltaY * orbit.sensitivity;
      if (orbit.pitch > 85.0f)  orbit.pitch = 85.0f;
      if (orbit.pitch < -2.0f)  orbit.pitch = -2.0f;
   }

   // --- Mouse Steering (tasto N) ---
   if (mouseSteering.isActive) {
      mouseSteering.currentAngle -= deltaX * mouseSteering.sensitivity;
      if (mouseSteering.currentAngle > 35.0f) mouseSteering.currentAngle = 35.0f;
      if (mouseSteering.currentAngle < -35.0f) mouseSteering.currentAngle = -35.0f;
      myCar.setMouseSteeringTarget(mouseSteering.currentAngle);  // era setSteeringAngleDirect
   }

   ignoreNextMotion = true;
   // Warp solo se almeno una modalità è attiva
   engine->warpMouse(cx, cy);
}

// /////////////
// Debug method
// ////////////
void printSceneGraphWithPosition(Node* node, int level = 0) {
    if (!node) return;
    std::string indent(level * 4, ' ');
    std::string branch = (level == 0) ? "ROOT " : "|__ ";
    glm::mat4 worldMatrix = node->getWorldFinalMatrix();
    float x = worldMatrix[3][0];
    float y = worldMatrix[3][1];
    float z = worldMatrix[3][2];

    std::cout << indent << branch << "ID:" << node->getId() << " '" << node->getName() << "'";
    if (dynamic_cast<Mesh*>(node)) std::cout << " [MESH]";
    else if (dynamic_cast<Light*>(node)) std::cout << " [LIGHT]";
    else if (dynamic_cast<Camera*>(node)) std::cout << " [CAMERA]";
    std::cout << std::fixed << std::setprecision(5) << " -> Pos: (" << x << ", " << y << ", " << z << ")" << std::endl;

    for (unsigned int i = 0; i < node->getNumChildren(); i++) {
        printSceneGraphWithPosition(node->getChild(i), level + 1);
    }
}


int main(int argc, char* argv[]) {
    // --- INIZIO ---

    engine = &Eng::Base::getInstance();
    if (!engine->init(argc, argv)) return -1;

    engine->createWindow(800, 600, 100, 100, "Car Simulator");
    engine->enableFPS();



    // Setup Callback
    engine->setKeyboardCallback(keyboardCallback);
    engine->setKeyboardUpCallback(keyboardUpCallback);
    engine->setSpecialCallback(specialCallback);
    engine->setDisplayCallback(displayCallback);
    engine->setReshapeCallback(reshapeCallback);
    engine->setMouseMotionCallback(mouseMotionCallback);

    camera = new PerspectiveCamera("MainCam", 45.0f, 800.0f / 600.0f, 1.0f, 5000.0f);
   // --- SETUP VISTA FRONTALE ---

   // Hard-coded
    //camera->rotate(-25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    //mainCameraHome = camera->getM(); // salva posizione iniziale della camera mobile

    list = new List();
    root = new Node("Root");
    Node* scena = ovoreader.readFile("macchina.ovo", "texture/");

  

    if (scena) {
       std::cout << "OVO caricato con successo! Aggiungo alla scena." << std::endl;
       root = scena;
       root->addChild(camera);
       printSceneGraphWithPosition(root);
       Node* c = root->findByName("Car");
       if (c)
       {
          myCar.init(c);
       }

       Node* foundNode = root->findByName("Plane"); // Trova il piano dall'OVO
       if (foundNode) {
          groundMesh = dynamic_cast<Mesh*>(foundNode);
          // Assicurati che la texture sia impostata su GL_REPEAT (di solito lo è di default in texture.cpp)
       }


    }
    else {
       std::cerr << "Errore critico: impossibile caricare tavolo.ovo" << std::endl;
    }

    Node* carNode = root->findByName("Car");

    // luci figlie di macchina per spostamento migliore
    if (carNode) {
       
       std::string lightNames[] = { "Omni", "Omni001", "Omni002", "Omni003", "Omni004" };

       for (const std::string& name : lightNames) {
          Node* currentLight = root->findByName(name);

          if (currentLight) {
             if (currentLight->getParent()) {
                currentLight->getParent()->removeChild(currentLight);
             }
             carNode->addChild(currentLight);
          }
          else {
             std::cerr << "Attenzione: Impossibile trovare la luce '" << name << "' nello Scene Graph!" << std::endl;
          }
       }
    }
    else {
       std::cerr << "Attenzione: Impossibile trovare 'Car' nello Scene Graph!" << std::endl;
    }


    //root->removeChild(root->findByName("Omni"));
    printSceneGraphWithPosition(root);
    //engine->setLighting(false);


    engine->update();
    engine->free();

    // Cleanup
    delete list;

    delete camera;
    return 0;
}
