#pragma once
#include "node.h"
#include "wheel.h"

class Car
{
public:
   Car();

   void init(Node* carNode, int startX, int startZ);

   bool startEngine();
   bool turnOffEngine();

   void setAccelerating(bool isAccelerating);
   void setBraking(bool isBraking);
   void setSteering(double angle);

   void update(double deltaTime);

   glm::mat4 getWorldMatrix() const;

   const std::string wheelNames[4] = { "RuotaAD", "RuotaAS", "RuotaPD", "RuotaPS" };
   const std::string rimNames[4] = { "CerchioneAD", "CerchioneAS", "CerchionePD", "CerchionePS" };

private:
   Node* carModel = nullptr;
   Wheel  wheels[4];

   float      originalY = 0.0f;
   glm::vec3  originalScale = glm::vec3(1.0f);

   double maxSpeed;
   double currSpeed;
   double accelerationFactor;
   double brakingFactor;
   double friction;

   double posX;
   double posZ;

   double carHeading = 90.0;
   double steeringAngle = 0.0;

   bool isEngineOn = false;
   bool isAccelerating = false;
   bool isBraking = false;
};