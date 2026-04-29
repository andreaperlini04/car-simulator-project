#pragma once
#include "CarState.h"

/**
 * @brief Traduce l'input raw (tastiera/mouse) nello stato discreto per CarPhysics.
 * Isola l'hardware dalla simulazione per garantire determinismo (es. per replay o step fisici fissi).
 */
class CarInputController
{
public:
   void setAccelerating(bool v) { inputState.isAccelerating = v; }
   void setBraking(bool v) { inputState.isBraking = v; }
   void setHandbrake(bool v) { inputState.isHandbrake = v; }
   void setSteeringLeft(bool v) { isSteeringLeft = v; }
   void setSteeringRight(bool v) { isSteeringRight = v; }

   // Valutare un filtro passa-basso o un PID per ammorbidire il ritorno al centro.
   void setMouseSteering(bool active);
   void setMouseSteeringTarget(double angle);
   void setSteeringAngleDirect(double angle);

   /**
    * @brief Applica smoothing all'angolo di sterzo.
    * L'input digitale (tastiera) viene interpolato nel tempo,
    * mentre l'input analogico (mouse) bypassa il lerp per reattività immediata.
    */
   void updateSteeringAngle(double deltaTime);

   const CarInputState& getState() const { return inputState; }

private:
   CarInputState inputState;

   bool isSteeringLeft = false;
   bool isSteeringRight = false;

   bool   isMouseSteering = false;
   double mouseSteeringTarget = 0.0;

   static constexpr double STEERING_SPEED = 50.0;
};