#pragma once

#include "node.h"
#include "CarInputController.h"
#include "CarPhysics.h"
#include "CarRenderer.h"

/**
 * @brief Facade per l'architettura del veicolo.
 * Impone il disaccoppiamento tra la simulazione fisica deterministica (fixed-step)
 * e l'aggiornamento visivo dipendente dal framerate effettivo.
 */
class Car
{
public:
   Car(double maxSpeed,
      double accelerationFactor,
      double brakingFactor,
      double friction,
      double reverseGearMaxSpeed);

   void init(Node* carNode);

   bool startEngine();
   bool turnOffEngine();
   bool isEngineStarted() const;

   void setAccelerating(bool v);
   void setBraking(bool v);
   void setHandbrake(bool v);
   void setSteeringLeft(bool v);
   void setSteeringRight(bool v);

   // Override diretti per input analogici (es. gamepad o mouse)
   void setSteeringAngleDirect(double angle);
   void setMouseSteeringTarget(double angle);
   void setMouseSteering(bool active);

   /**
    * @brief Avanza l'integrazione della fisica.
    * Deve girare in un loop accumulatore a frequenza fissa per garantire
    * determinismo ed evitare instabilità numeriche sulle collisioni.
    * @param fixedDt Delta time fisso in secondi (es. 1.0/60.0).
    */
   void stepPhysics(double fixedDt);

   /**
    * @brief Sincronizza il transform nel scene graph e interpola le animazioni.
    * Da eseguire una singola volta per frame, a valle dello step fisico.
    * @param frameDt Delta time reale in secondi. Non influisce sulla simulazione.
    */
   void updateRenderer(double frameDt);

   double    getCurrSpeedAbs() const;
   double    getMaxSpeed()     const;
   glm::mat4 getWorldMatrix()  const;

private:
   CarInputController inputController;
   CarPhysics         physics;
   CarRenderer        renderer;
};