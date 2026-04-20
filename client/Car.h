#pragma once
#include "node.h"
#include "CarInputController.h"
#include "CarPhysics.h"
#include "CarRenderer.h"

/**
 * @brief Facade che compone CarInputController, CarPhysics e CarRenderer.
 *
 * Questa classe espone la stessa API pubblica della classe Car originale, ma
 * delega ogni responsabilita' al sottosistema appropriato.
 *
 * FIXED TIMESTEP:
 *   La fisica e' ora separata dal rendering. Il loop principale deve chiamare:
 *     1. stepPhysics(FIXED_DT)  — N volte per frame, dentro il loop accumulatore
 *     2. updateRenderer(frameDt) — 1 volta per frame, al frame-rate reale
 *
 *   Questo garantisce che la simulazione sia deterministica e indipendente
 *   dal frame-rate: spike di rendering non causano "teletrasporti" fisici.
 */
class Car
{
public:
   Car(double maxSpeed,
      double accelerationFactor,
      double brakingFactor,
      double friction,
      double reverseGearMaxSpeed);

   //  Ciclo di vita
   void init(Node* carNode);

   bool startEngine();
   bool turnOffEngine();
   bool isEngineStarted() const;

   // Input (delegati a CarInputController)
   void setAccelerating(bool v);
   void setBraking(bool v);
   void setHandbrake(bool v);
   void setSteeringLeft(bool v);
   void setSteeringRight(bool v);
   void setSteeringAngleDirect(double angle);
   void setMouseSteeringTarget(double angle);
   void setMouseSteering(bool active);

   /**
    * @brief Esegue UN passo fisico a timestep fisso.
    *
    * Deve essere chiamato all'interno del loop accumulatore in displayCallback,
    * sempre con lo stesso valore fisso (es. 1.0/60.0).
    * Aggiorna: sterzata (CarInputController) + fisica (CarPhysics).
    *
    * @param fixedDt  Timestep fisso in secondi (es. PHYSICS_FIXED_DT).
    */
   void stepPhysics(double fixedDt);

   /**
    * @brief Aggiorna la rappresentazione visiva dell'auto.
    *
    * Deve essere chiamato UNA VOLTA per frame, dopo tutti i passi fisici.
    * Aggiorna la matrice mondo del modello 3D e l'animazione delle ruote.
    *
    * @param frameDt  Tempo reale trascorso dall'ultimo frame (seconds).
    *                 Usato SOLO per animazioni visive (rotazione ruote), NON per fisica.
    */
   void updateRenderer(double frameDt);

   // Lettura
   double    getCurrSpeedAbs() const;
   double    getMaxSpeed()     const;
   glm::mat4 getWorldMatrix()  const;

private:
   CarInputController inputController;
   CarPhysics         physics;
   CarRenderer        renderer;
};