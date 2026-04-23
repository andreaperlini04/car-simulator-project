#pragma once

/**
 * @brief Payload dei comandi generato dall'input e consumato dal solver fisico.
 */
struct CarInputState
{
   bool isAccelerating = false;
   bool isBraking = false;
   bool isHandbrake = false;
   double steeringAngle = 0.0; // Positivo = sinistra
};

/**
 * @brief Snapshot cinematico per il decoupling fisica/rendering.
 */
struct CarPhysicsState
{
   double posX = 0.0;
   double posY = 0.0;
   double posZ = 0.0;

   double carHeading = 90.0; // Gradi, world-space

   double currSpeed = 0.0;
   double velX = 0.0;
   double velZ = 0.0;
};