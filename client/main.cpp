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

// --- GLOBALI ---
Eng::Base* engine;
Camera* camera;
List* list;
Node* root;
OvoReader ovoreader{};



glm::mat4 mainCameraHome{ 1.0f };
bool isRotationMode = false; // false = MUOVI, true = RUOTA
bool isPresetView = false;   // true quando si è in una delle telecamere fisse


void specialCallback(int key, int x, int y) {
    // Delega la gestione delle frecce alla classe logica
    
}

void displayCallback() {
    
    engine->postRedisplay();
}

void keyboardCallback(unsigned char key, int x, int y) {
    
    engine->postRedisplay();
}

void reshapeCallback(int width, int height) {
    if (height == 0) height = 1;
    PerspectiveCamera* pCam = dynamic_cast<PerspectiveCamera*>(camera);
    if (pCam) {
        pCam->setAspectRatio((float)width, (float)height);
    }
}

// per debuggare 
/*
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
*/

int main(int argc, char* argv[]) {
    // --- INIZIO ---

    engine = &Eng::Base::getInstance();
    if (!engine->init(argc, argv)) return -1;

    engine->createWindow(800, 600, 100, 100, "Car Simulator");
    engine->enableFPS();



    // Setup Callback
    engine->setKeyboardCallback(keyboardCallback);
    engine->setSpecialCallback(specialCallback);
    engine->setDisplayCallback(displayCallback);
    engine->setReshapeCallback(reshapeCallback);

    camera = new PerspectiveCamera("MainCam", 45.0f, 800.0f / 600.0f, 1.0f, 5000.0f);
   // --- SETUP VISTA FRONTALE ---

   // Hard-coded
    camera->translate(glm::vec3(0.0f, 50.0f, 50.0f));
    camera->rotate(-25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    mainCameraHome = camera->getM(); // salva posizione iniziale della camera mobile

    list = new List();
    root = new Node("Root");

    




    engine->update();
    engine->free();

    // Cleanup
    delete list;

    delete camera;
    return 0;
}
