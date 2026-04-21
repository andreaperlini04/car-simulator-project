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
 * @brief Gestisce il ciclo di vita dell'applicazione Car Simulator.
 *
 * Raccoglie tutto lo stato che era disperso come variabili globali in main.cpp
 * e lo incapsula in una singola classe con responsabilita' chiara:
 *   - Inizializzazione engine, finestra e scena
 *   - Loop di gioco (fisica + render)
 *   - Gestione input (tastiera e mouse)
 *
 * I callback richiesti dall'engine (che accetta solo free-function pointer)
 * vengono forniti da wrapper statici in Application.cpp, che delegano
 * all'istanza di Application tramite un puntatore locale s_app.
 */
class Application
{
public:
   Application();

   /**
    * @brief Inizializza engine, finestra e scena.
    * @return false se qualcosa fallisce (OVO non trovato, ecc.)
    */
   bool init(int argc, char* argv[]);

   /**
    * @brief Avvia il main loop (bloccante fino alla chiusura della finestra).
    */
   void run();

   // ------------------------------------------------------------------
   // Callback handlers — chiamati dai wrapper statici in Application.cpp.
   // Non sono pensati per essere invocati direttamente dall'esterno.
   // ------------------------------------------------------------------
   void onDisplay();
   void onKeyboard(unsigned char key, int x, int y);
   void onKeyboardUp(unsigned char key);
   void onSpecial(int key, int x, int y);
   void onReshape(int width, int height);
   void onMouseMotion(int x, int y);

private:

   // ---- Strutture di stato interno --------------------------------

   struct OrbitCameraState {
      float yaw = 0.0f;    ///< Rotazione orizzontale (gradi)
      float pitch = 30.0f;   ///< Inclinazione verticale (gradi)
      float radius = 100.0f;  ///< Distanza dalla macchina (unita' di gioco)
      float sensitivity = 0.1f;    ///< Sensibilita' del mouse
      bool  isMotionCameraActivated = false;
   };

   struct MouseSteeringState {
      bool  isActive = false;
      float sensitivity = 0.1f;
      float currentAngle = 0.0f;  ///< Angolo di sterzo corrente (gradi)
   };

   struct CameraSelection {
      enum Position { BEHIND = 1, LEFT = 2, RIGHT = 3 };
      Position current = BEHIND;
   };

   // ---- Engine & Scena --------------------------------------------

   Eng::Base& engine;          ///< Riferimento al singleton del motore
   Camera* camera = nullptr; ///< Observer (ownership: root)
   std::unique_ptr<List> list;
   std::unique_ptr<Node> root;
   OvoReader             ovoreader;
   Mesh* groundMesh = nullptr; ///< Observer (ownership: root)

   // ---- Simulazione -----------------------------------------------

   Car  myCar;
   bool isGameStarted = false;

   // ---- Timing (fixed-timestep loop) ------------------------------

   std::chrono::steady_clock::time_point lastFrameTime;
   bool   isFirstFrame = true;
   double physicsAccumulator = 0.0;

   // ---- Stato input / camera --------------------------------------

   OrbitCameraState   orbit;
   CameraSelection    selectedCamera;
   MouseSteeringState mouseSteering;
   bool               ignoreNextMotion = false;

   // ---- Metodi privati --------------------------------------------

   bool loadScene();
   void reparentLightsToCar();

   void        updateCameraFollow(CameraSelection::Position pos);
   void        drawCenteredText(const std::string& text, float yOffset,
      float r, float g, float b);
   void        printCustomText();
   std::string getSpeedToDisplay() const;

   // Debug
   static void printSceneGraphWithPosition(Node* node, int level = 0);
};