#pragma once

#include "engine.h"
#include "camera.h"
#include "mesh.h"
#include "ovoReader.h"
#include "perspectiveCamera.h"
#include "Car.h"
#include "WorldConfig.h"

#include <memory>
#include <chrono>
#include <string>

/**
 * @brief Root controller del simulatore.
 * * Centralizza lo stato dell'applicazione per facilitare il reset della scena.
 * Risolve i requisiti dei callback C-style dell'engine appoggiandosi a wrapper statici esterni.
 */
class Application
{
public:
   Application();

   /**
    * @brief Inizializza engine, contesto grafico e grafo della scena.
    * @return false su errori fatali (es. asset OVO mancanti).
    */
   bool init(int argc, char* argv[]);

   /**
    * @brief Entra nel main loop (bloccante).
    */
   void run();

   // Delegati dai wrapper C-style in Application.cpp
   void onDisplay();
   void onKeyboard(unsigned char key, int x, int y);
   void onKeyboardUp(unsigned char key);
   void onSpecial(int key, int x, int y);
   void onReshape(int width, int height);
   void onMouseMotion(int x, int y);

private:

   struct OrbitCameraState {
      float yaw = 0.0f;
      float pitch = 30.0f;
      float radius = 100.0f;
      float sensitivity = 0.1f;
      bool  isMotionCameraActivated = false;
   };

   // anche se non è bellissimo da vedere e usare
   struct MouseSteeringState {
      bool  isActive = false;
      float sensitivity = 0.1f;
      float currentAngle = 0.0f;
   };

   struct CameraSelection {
      enum Position { BEHIND = 1, LEFT = 2, RIGHT = 3 };
      Position current = BEHIND;
   };

   Eng::Base& engine;

   // Puntatori observer, la deallocazione è gestita dal nodo root
   Camera* camera = nullptr;
   Mesh* groundMesh = nullptr;

   std::unique_ptr<List> list;
   std::unique_ptr<Node> root;
   OvoReader            ovoreader;

   Car  myCar;
   bool isGameStarted = false;

   // Stato per fixed-timestep: garantisce fisica deterministica indipendente dal framerate
   std::chrono::steady_clock::time_point lastFrameTime;
   bool   isFirstFrame = true;
   double physicsAccumulator = 0.0;

   OrbitCameraState   orbit;
   CameraSelection    selectedCamera;
   MouseSteeringState mouseSteering;

   // Workaround: evita salti di camera ignorando il delta quando si forza il cursore a centro schermo
   bool               ignoreNextMotion = false;

   bool loadScene();

   // Fissa le luci allo spazio locale del telaio per preservare l'allineamento in curva
   void reparentLightsToCar();

   void        updateCameraFollow(CameraSelection::Position pos);
   void        drawCenteredText(const std::string& text, float yOffset, float r, float g, float b);
   void        printCustomText();
   std::string getSpeedToDisplay() const;

   static void printSceneGraphWithPosition(Node* node, int level = 0);
};