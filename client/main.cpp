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

// --- GLOBALI ---
Eng::Base* engine;
Camera* camera;
List* list;
Node* root;
Node* omniLight;
OvoReader ovoreader{};
Car myCar;
bool isGameStarted = false;
Mesh* groundMesh = nullptr;
int selectedCamera = 1; // Follows the car from behind

// Timer
auto lastFrameTime = std::chrono::steady_clock::now();
bool isFirstFrame = true;


glm::mat4 mainCameraHome{ 1.0f };
void updateCameraFollow(int idx) {
    if (!camera) return;

    glm::mat4 carMatrix = myCar.getWorldMatrix();
    glm::vec3 carPosition = glm::vec3(carMatrix[3]);

    // Estrazione assi locali della macchina
    glm::vec3 carRight = glm::normalize(glm::vec3(carMatrix[0])); // X axis (Left/Right)
    glm::vec3 carUp = glm::normalize(glm::vec3(carMatrix[1])); // Y axis (Up/Down)
    glm::vec3 carForward = glm::normalize(glm::vec3(carMatrix[2])); // Z axis (Back/Forward)

    float distanceBehind = 0.0f;
    float distanceSide = 0.0f; 
    float heightAbove = 0.0f;

    glm::vec3 cameraPos{ 1.f };
    glm::vec3 cameraTarget{ 1.f };
    glm::mat4 viewMatrix{ 1.f };

    if (idx == 1) {
        // Camera da DIETRO
        distanceBehind = 50.0f;
        heightAbove = 50.0f; 

        cameraPos = carPosition + (carForward * distanceBehind) + (carUp * heightAbove);
        cameraTarget = carPosition + (carUp * 2.0f);
        viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (idx == 2) {
        // Camera lato SINISTRO
        distanceSide = 40.0f;
        heightAbove = 0.0f; 

        // Sottraiamo carRight per posizionarci a sinistra della macchina
        cameraPos = carPosition - (carRight * distanceSide) + (carUp * heightAbove);
        cameraTarget = carPosition + (carUp * 2.0f); 

        viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if (idx == 3) {
        //  Camera lato DESTRO
        distanceSide = 40.0f;
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

   if (groundMesh) {
      // 1. Logica Posizione (Geometria):
      // Sposta il piano sotto la macchina affinché non "finisca" mai.
      glm::mat4 carMat = myCar.getWorldMatrix();
      float carX = carMat[3][0];
      float carZ = carMat[3][2];
      float groundY = groundMesh->getM()[3][1]; // Mantieni altezza originale

      // Sposta la mesh fisica
      groundMesh->setM(glm::translate(glm::mat4(1.0f), glm::vec3(carX, groundY, carZ)));

      // 2. Logica Texture (Illusione Movimento):
      // Calcola l'offset opposto al movimento.
      Material* mat = groundMesh->getMaterial();
      if (mat) {
         float textureFactor = 0.05f; // Regola quanto è "fitta" la texture

         // Calcola matrice con GLM (Client side)
         glm::mat4 texAnim = glm::translate(glm::mat4(1.0f),
            glm::vec3(carX * textureFactor, -carZ * textureFactor, 0.0f));

         // Passa la matrice all'Engine. Il Client NON chiama glLoadMatrixf.
         mat->setTextureMatrix(texAnim);
      }

     
   }

   // TODO mettere luce in una variabile
   glm::mat4 carMat = myCar.getWorldMatrix();
   float carX = carMat[3][0];
   float carZ = carMat[3][2];
   float lighty = omniLight->getM()[3][1]; // Mantieni altezza originale

   // Sposta la mesh fisica
   omniLight->setM(glm::translate(glm::mat4(1.0f), glm::vec3(carX, lighty, carZ)));

   updateCameraFollow(selectedCamera);
   list->clear();
   list->pass(root, glm::mat4(1.0f));

   // STARTING TEXT
   if(!isGameStarted){
       drawCenteredText("Welcome to the Car Simulator", 0.0f, 0.2f, 1.0f, 0.2f); // Verde Lime
       drawCenteredText("Press [E] to turn on the engine", -30.0f, 1.0f, 1.0f, 1.0f); // Bianco
   }
   else { // MENU
       engine->clearScreenText();
       engine->addToScreenText("[1] Main Camera");
       engine->addToScreenText("[2-3] Left/Right Camera");
       engine->addToScreenText("[W-S] Accelerate/Decelerate the car");
       engine->addToScreenText("[A-D] Turn Left/Rigth");
       engine->addToScreenText("\n");
       
       if (myCar.isEngineStarted()) {
           engine->addToScreenText("[T] Turn off the engine");
       } else {
           engine->addToScreenText("[E] Turn on the engine");
       }
       //engine->addToScreenText("[] ");
   }
   

   engine->setRenderList(list);
   engine->setMainCamera(camera);
   engine->postRedisplay();
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
  
   case 'a':
       if(isGameStarted)
            myCar.setSteeringLeft(true); 
       break;
   case 'd':
       if(isGameStarted)
            myCar.setSteeringRight(true); 
      break;
   case '1':
       selectedCamera = 1;
       break;
   case '2':
       selectedCamera = 2;
       break;
   case '3':
       selectedCamera = 3;

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
   case 'a':
       myCar.setSteeringLeft(false);
       break;
   case 'd':
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

// Debug method

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
    std::cout << std::fixed << std::setprecision(1) << " -> Pos: (" << x << ", " << y << ", " << z << ")" << std::endl;

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

    camera = new PerspectiveCamera("MainCam", 45.0f, 800.0f / 600.0f, 1.0f, 5000.0f);
   // --- SETUP VISTA FRONTALE ---

   // Hard-coded
    camera->translate(glm::vec3(0.0f, 50.0f,100.0f));
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
       Node* t = root->findByName("Car");
       if (t)
       {
          myCar.init(t, 0, 0);
       }

       Node* foundNode = root->findByName("Piano"); // Trova il piano dall'OVO
       if (foundNode) {
          groundMesh = dynamic_cast<Mesh*>(foundNode);
          // Assicurati che la texture sia impostata su GL_REPEAT (di solito lo è di default in texture.cpp)
       }
    }
    else {
       std::cerr << "Errore critico: impossibile caricare tavolo.ovo" << std::endl;
    }

    omniLight = root->findByName("Omni");

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
