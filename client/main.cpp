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
List* reflectionList = nullptr;
std::vector<Node*> reflectionNodesCache;
Node* root;
OvoReader ovoreader{};
Node* tavoloNode;



glm::mat4 mainCameraHome{ 1.0f };
bool isRotationMode = false; // false = MUOVI, true = RUOTA
bool isPresetView = false;   // true quando si è in una delle telecamere fisse

void drawCenteredText(std::string text, float yOffset, float r, float g, float b) {
    int winW = engine->getWindowWidth();
    int winH = engine->getWindowHeight();
    int textWidth = engine->getTextWidth(text);

    float x = (winW - textWidth) / 2.0f;
    float y = (winH / 2.0f) + yOffset;
    engine->addString(x, y, text, r, g, b);
}

void specialCallback(int key, int x, int y) {
    // Delega la gestione delle frecce alla classe logica
    
}

glm::mat4 getReflectionMatrix(float planeHeight) {
   glm::mat4 mat(1.0f);
   // 1. Sposta al piano
   mat = glm::translate(mat, glm::vec3(0.0f, planeHeight, 0.0f));
   // 2. Specchia l'asse Y
   mat = glm::scale(mat, glm::vec3(1.0f, -1.0f, 1.0f));
   // 3. Sposta indietro
   mat = glm::translate(mat, glm::vec3(0.0f, -planeHeight, 0.0f));

   return mat;
}


void displayCallback() {
    static float angle = 0.0f;
    angle += 0.5f;

    // Aggiorna animazione disco in mano (hover + bobbing)
    

    // Preparazione lista di rendering
    list->clear();
    list->pass(root, glm::mat4(1.0f));

    engine->setRenderList(list);
    engine->setMainCamera(camera);

    engine->clearScreenText();

    // ==== MENU ====
    if (isRotationMode)
        engine->addToScreenText("MODALITA': ROTAZIONE (WASD)");
    else
        engine->addToScreenText("MODALITA': MOVIMENTO (WASD)");

    engine->addToScreenText("[M] Cambia Modalita' | [P] Camera principale | [ESC] Esci");
    engine->addToScreenText("Freccia SX/DX: cambia piolo");
    engine->addToScreenText("Freccia SU: prendi disco");
    engine->addToScreenText("Freccia GIU: lascia");
    engine->addToScreenText("[R] Reset | [U] Undo | [Y] Redo");
    engine->addToScreenText("[1-4] Camera Presets");

    // ==== VITTORIA ====
    

    if(reflectionList) {
       reflectionList->clear();

       float tableHeight = 16.5f;
       glm::mat4 reflectMat = getReflectionMatrix(tableHeight);

       // USIAMO LA CACHE 
       for (Node* node : reflectionNodesCache) {
          if (node) {
             reflectionList->pass(node, reflectMat);
          }
       }

       engine->setReflectionList(reflectionList);
    }

    engine->postRedisplay();
}

void keyboardCallback(unsigned char key, int x, int y) {
    float moveSpeed = 1.0f;
    float rotSpeed = 2.0f;

    switch (key) {
    case 'm': case 'M':
        isRotationMode = !isRotationMode;
        break;

        // --- CAMERA PRINCIPALE ---
    case 'p':
    case 'P':
        camera->setM(mainCameraHome);
        isPresetView = false;
        std::cout << "Camera principale attiva (mobile)" << std::endl;
        break;

        // --- MOVIMENTO / ROTAZIONE (WASD) ---
    case 'w':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(rotSpeed, glm::vec3(1, 0, 0));
        else                camera->translate(glm::vec3(0, 0, -moveSpeed));
        break;
    case 's':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(-rotSpeed, glm::vec3(1, 0, 0));
        else                camera->translate(glm::vec3(0, 0, moveSpeed));
        break;
    case 'a':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(rotSpeed, glm::vec3(0, 1, 0));
        else                camera->translate(glm::vec3(-moveSpeed, 0, 0));
        break;
    case 'd':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(-rotSpeed, glm::vec3(0, 1, 0));
        else                camera->translate(glm::vec3(moveSpeed, 0, 0));
        break;
    case 'q':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(rotSpeed, glm::vec3(0, 0, 1));
        else                camera->translate(glm::vec3(0, moveSpeed, 0));
        break;
    case 'e':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(-rotSpeed, glm::vec3(0, 0, 1));
        else                camera->translate(glm::vec3(0, -moveSpeed, 0));
        break;

        // --- TELECAMERE FISSE ---

    case 'r': case 'R':
    {
        std::cout << "[GAME] Ricaricamento livello..." << std::endl;

        // Pulizia vecchia scena
        if (root) {
            root->removeChild(camera);
            delete root;
            root = nullptr;
        }

        // Caricamento nuova scena
        tavoloNode = ovoreader.readFile("tavolo.ovo", "texture/");

        if (tavoloNode) {
            root = tavoloNode;
            root->addChild(camera);



            // RESET SCENA
            

            reflectionNodesCache.clear();

            // Cache Base
            Node* baseNode = root->findByName("Base");
            if (baseNode) reflectionNodesCache.push_back(baseNode);

            // Cache Pali
            for (int i = 1; i <= 3; i++) {
               Node* palo = root->findByName("Palo" + std::to_string(i));
               if (palo) reflectionNodesCache.push_back(palo);
            }

            // Cache Dischi
            for (int i = 1; i <= 7; i++) {
               Node* disco = root->findByName("Disco" + std::to_string(i));
               if (disco) reflectionNodesCache.push_back(disco);
            }

            // Cache Luci
            for (int i = 1; i <= 5; i++) {
               Node* luce = root->findByName("Omni00" + std::to_string(i));
               if (luce) reflectionNodesCache.push_back(luce);
            }



        }
        else {
            std::cerr << "[ERROR] Impossibile ricaricare il file .ovo!" << std::endl;
            exit(-1);
        }
        break;
    }

    // --- UNDO/REDO ---
    
    }

    // Evita alla camera di poter uscire dalla scena
    
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

    engine->createWindow(800, 600, 100, 100, "Tower of Hanoi");
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
    reflectionList = new List();
    root = new Node("Root");

    tavoloNode = ovoreader.readFile("tavolo.ovo", "texture/");

    if (tavoloNode) {
        std::cout << "OVO caricato con successo! Aggiungo alla scena." << std::endl;
        root = tavoloNode;
        root->addChild(camera);


        root = tavoloNode;
        //Node* target = root->findByName("Spot001.Target");
        //root->removeChild(root->findByName("Omni001"));

        Node* base_tavolo = root->findByName("base_tavolo");
        Mesh* base_tavolo_mesh = dynamic_cast<Mesh*>(base_tavolo);
        base_tavolo_mesh->getMaterial()->setTransparency(0.5f);
        //root->removeChild(root->findByName("Omni004"));
        //root->removeChild(root->findByName("Omni003"));
        //root->removeChild(root->findByName("Omni002"));
        //root->removeChild(root->findByName("Omni005"));
        //root->removeChild(root->findByName("Omni001"));

        // TESTING TROPPE LUCI
        /*
        OmnidirectionalLight*  o1 = new OmnidirectionalLight();
        OmnidirectionalLight* o2 = new OmnidirectionalLight();
        OmnidirectionalLight* o3 = new OmnidirectionalLight();
        OmnidirectionalLight* o4 = new OmnidirectionalLight();
        root->addChild(o1);
        root->addChild(o2);
        root->addChild(o3);
        root->addChild(o4);
        */



        //root->addChild(light);
        root->addChild(camera);
        printSceneGraphWithPosition(root);
        // === INIZIALIZZAZIONE SCENA E LOGICA HANOI ===
        // Passiamo Camera e Engine al costruttore
        // Dalla root percorre il grafo
        reflectionNodesCache.clear();

        // Torre riflessa
        // Cache Base
        Node* baseNode = root->findByName("Base");
        if (baseNode) reflectionNodesCache.push_back(baseNode);

        // Cache Pali
        for (int i = 1; i <= 3; i++) {
           Node* palo = root->findByName("Palo" + std::to_string(i));
           if (palo) reflectionNodesCache.push_back(palo);
        }

        // Cache Dischi
        for (int i = 1; i <= 7; i++) {
           Node* disco = root->findByName("Disco" + std::to_string(i));
           if (disco) reflectionNodesCache.push_back(disco);
        }

        // Cache Luci
        for (int i = 1; i <= 5; i++) {
           Node* luce = root->findByName("Omni00" + std::to_string(i));
           if (luce) reflectionNodesCache.push_back(luce);
        }

        std::cout << "\n--- STRUTTURA SCENA ---" << std::endl;
        printSceneGraphWithPosition(root);
        std::cout << "-----------------------\n" << std::endl;

    }
    else {
        std::cerr << "Errore critico: impossibile caricare tavolo.ovo" << std::endl;
    }




    engine->update();
    engine->free();

    // Cleanup
    delete list;

    delete camera;
    return 0;
}
