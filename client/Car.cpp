#define _USE_MATH_DEFINES

#include "Car.h"
#include "engine.h"
#include <iostream>
#include <cmath>

Car::Car()
{
   this->isEngineOn = false;
   this->isAccelerating = false;
   this->isBraking = false;

   this->maxSpeed = 100.0;
   this->currSpeed = 0.0;
   this->accelerationFactor = 8.0;
   this->brakingFactor = 20.0;
   this->friction = 10.0;

   this->carHeading = 90.0;
   this->steeringAngle = 0.0;

   this->posX = 0.0;
   this->posZ = 0.0;
}

bool Car::startEngine() { return this->isEngineOn = true; }
bool Car::turnOffEngine() { return this->isEngineOn = false; }
bool Car::isEngineStarted() const { return this->isEngineOn; }

void Car::setAccelerating(bool v) { this->isAccelerating = v; }
void Car::setBraking(bool v) { this->isBraking = v; }
void Car::setSteeringRight(bool isSteeringRigth) { this->isSteeringRight = isSteeringRigth; }
void Car::setSteeringLeft(bool isSteeringLeft) { this->isSteeringLeft = isSteeringLeft; }


void Car::update(double deltaTime)
{
   if (deltaTime < 0 || deltaTime > 1) return;

   // Accelerating 
   if (isEngineOn && isAccelerating) {
      currSpeed += accelerationFactor * deltaTime;
   }
   // Braking
   else if (isBraking) {
      if (isEngineOn)
         currSpeed -= brakingFactor * deltaTime;
      else {
         if (currSpeed > 0) {
            currSpeed -= brakingFactor * deltaTime;
         }
         else if (currSpeed < 0) {
            currSpeed += brakingFactor * deltaTime;
         }
      }
   }
   else {
      applyFriction(deltaTime);
   }

   if (currSpeed > maxSpeed) currSpeed = maxSpeed;
   if (currSpeed < -maxSpeed) currSpeed = -maxSpeed;

   updateSteeringAngle(deltaTime);
   double turnRate = steeringAngle * currSpeed * 0.05;
   double grip = friction / (std::abs(currSpeed) * 0.25 + 1.0);
   if (grip > 1.0) grip = 1.0;

   carHeading += turnRate * grip * deltaTime;

   double carHeadingRad = carHeading * (M_PI / 180.0);
   posX += currSpeed * std::sin(carHeadingRad) * deltaTime;
   posZ += currSpeed * std::cos(carHeadingRad) * deltaTime;

   if (carModel != nullptr) {
      glm::mat4 newMatrix = glm::mat4(1.0f);
     
      newMatrix = glm::translate(newMatrix, glm::vec3((float)posX, originalY, (float)posZ));
      newMatrix = glm::rotate(newMatrix, (float)carHeadingRad + 3.14159f, glm::vec3(0.0f, 1.0f, 0.0f));
      newMatrix = glm::scale(newMatrix, originalScale);
      carModel->setM(newMatrix);

      double distanceMoved = currSpeed * deltaTime;

      wheels[0].setSteeringAngle(steeringAngle);
      wheels[1].setSteeringAngle(steeringAngle);
      wheels[2].setSteeringAngle(0.0);
      wheels[3].setSteeringAngle(0.0);

      for (int i = 0; i < 4; i++) {
         wheels[i].updateRolling(distanceMoved);
         wheels[i].updateVisuals();
      }
   }
}

void Car::applyFriction(double deltaTime)
{
    if (currSpeed > 0) {
        currSpeed -= friction * deltaTime;
        if (currSpeed < 0) currSpeed = 0;
    }
    else if (currSpeed < 0) {
        currSpeed += friction * deltaTime;
        if (currSpeed > 0) currSpeed = 0;
    }
}

void Car::init(Node* passedNode, int startX, int startZ)
{
   this->posX = startX;
   this->posZ = startZ;

   Node* rootScene = passedNode;
   while (rootScene->getParent() != nullptr)
      rootScene = rootScene->getParent();

   this->carModel = rootScene->findByName("Car");
   if (!this->carModel) return;

   // 1. RECUPERIAMO ALTEZZA E SCALA DAL MODELLO .OVO
   glm::mat4 ovoMatrix = this->carModel->getWorldFinalMatrix();
   this->originalY = ovoMatrix[3][1];
   this->originalScale = glm::vec3(
      glm::length(glm::vec3(ovoMatrix[0])),
      glm::length(glm::vec3(ovoMatrix[1])),
      glm::length(glm::vec3(ovoMatrix[2]))
   );

   glm::mat4 startMatrix = glm::mat4(1.0f);
   startMatrix = glm::translate(startMatrix, glm::vec3((float)posX, originalY, (float)posZ));
   this->carModel->setM(startMatrix);

   for (int i = 0; i < 4; i++) {
      Node* ruota = rootScene->findByName(wheelNames[i]);
      Node* cerchione = rootScene->findByName(rimNames[i]);

      if (!ruota) continue;

      glm::mat4 origRuotaWorld = ruota->getWorldFinalMatrix();
      glm::mat4 relRuotaM = glm::inverse(startMatrix) * origRuotaWorld;

      if (ruota->getParent())
         ruota->getParent()->removeChild(ruota);
      this->carModel->addChild(ruota);
      ruota->setM(relRuotaM);

      wheels[i].init(ruota, 1.0f, 0.0, 0.0, 0.0);

     
   }
}

void Car::updateSteeringAngle(double const deltaTime) {
   if (isSteeringRight) {
      steeringAngle -= steeringSpeed * deltaTime;
      if (steeringAngle < -maxSteeringAngle)
         steeringAngle = -maxSteeringAngle;
   }
   else if (isSteeringLeft) {
      steeringAngle += steeringSpeed * deltaTime;
      if (steeringAngle > maxSteeringAngle)
         steeringAngle = maxSteeringAngle;
   }
   else if (steeringAngle > 0) {
      steeringAngle -= steeringSpeed * deltaTime;
      if (steeringAngle < 0.0)
         steeringAngle = 0.0;
   }
   else if (steeringAngle < 0) {
      steeringAngle += steeringSpeed * deltaTime;
      if (steeringAngle > 0.0)
         steeringAngle = 0.0;
   }
}

glm::mat4 Car::getWorldMatrix() const
{
   if (carModel) return carModel->getWorldFinalMatrix();
   return glm::mat4(1.0f);
}