#pragma once
#include "node.h"
#include "wheel.h"

class Car
{
public:
   Car(double maxSpeed, double accelerationFactor, double brakingFactor, double friction, double reverseGearMaxSpeed);

   void init(Node* carNode);

   bool startEngine();
   bool turnOffEngine();

   void setAccelerating(bool isAccelerating);
   void setBraking(bool isBraking);
   void setSteeringLeft(bool isSteeringLeft);
   void setSteeringRight(bool isSteeringRight);
   bool isEngineStarted() const;

   void update(double deltaTime);

   double getCurrSpeedAbs() const;

   double getMaxSpeed() const;


   void setSteeringAngleDirect(double angle);
   void setMouseSteeringTarget(double angle);
   void setMouseSteering(bool active);



   glm::mat4 getWorldMatrix() const;
   
   const std::string wheelNames[4] = { "RuotaAD", "RuotaAS", "RuotaPD", "RuotaPS" };

private:
   Node* carModel = nullptr;
   Wheel  wheels[4];

   double maxSpeed;
   double currSpeed;
   // --- Vettori per l'inerzia ---
   double velX = 0.0;
   double velZ = 0.0;
   double accelerationFactor;
   double brakingFactor;
   double friction;
   double reverseGearMaxSpeed;

   double posX;
   double posY;
   double posZ;

   double carHeading = 90.0;
   double steeringAngle = 0.0; // Positive angle => going left
   const double maxSteeringAngle = 35.0; // Degrees 

   bool isSteeringLeft = false;
   bool isSteeringRight = false;

   bool isEngineOn = false;
   bool isAccelerating = false;
   bool isBraking = false;

   const double steeringSpeed = 50.0; // Degree/s
   void updateSteeringAngle(double deltaTime);

   void applyFriction(double deltaTime);
   bool isMouseSteering = false;
   double mouseSteeringTarget = 0.0;
};