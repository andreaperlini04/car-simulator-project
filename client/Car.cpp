#define _USE_MATH_DEFINES

#include "Car.h"
#include "engine.h"
#include <iostream>
#include <cmath>



Car::Car(double maxSpeed, double accelerationFactor, double brakingFactor, double friction, double reverseGearMaxSpeed)
{
   this->isEngineOn = false;
   this->isAccelerating = false;
   this->isBraking = false;
   this->currSpeed = 0.0;
   this->velX = 0.0;
   this->velZ = 0.0;

   this->maxSpeed = maxSpeed;
   this->accelerationFactor = accelerationFactor;
   this->brakingFactor = brakingFactor;
   this->friction = friction;
   this->reverseGearMaxSpeed = reverseGearMaxSpeed;
       
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

double Car::getCurrSpeedAbs() const { return std::abs(this->currSpeed); }
double Car::getMaxSpeed() const { return this->maxSpeed; }



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
      {
         if (currSpeed > 0) {
            // Braking while moving forward: use full brake force
            currSpeed -= brakingFactor * deltaTime;
            if (currSpeed < 0) currSpeed = 0; // capped
         }
         else {
            // Reversing: a more moderate acceleration proportional to the reverse gear max speed ratio
            double reverseAccel = accelerationFactor * (reverseGearMaxSpeed / maxSpeed);
            currSpeed -= reverseAccel * deltaTime;
         }
      }
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
   if (currSpeed < -reverseGearMaxSpeed) currSpeed = -reverseGearMaxSpeed;

   updateSteeringAngle(deltaTime);
   double turnRate = steeringAngle * currSpeed * 0.020;
   double grip = friction / (std::abs(currSpeed) * 0.25 + 1.0);
   if (grip > 1.0) grip = 1.0;

   carHeading += turnRate * grip * deltaTime;

   // --- INERTIA ---
   // Calcoliamo i vettori di direzione in cui "guarda" l'auto
   double carHeadingRad = carHeading * (M_PI / 180.0);
   double forwardX = -std::sin(carHeadingRad);
   double forwardZ = -std::cos(carHeadingRad);

   // Calcoliamo la Velocità Ideale (dove la macchina vorrebbe andare in assenza di slittamento)
   double idealVelX = forwardX * currSpeed;
   double idealVelZ = forwardZ * currSpeed;

   // Calcoliamo l'aderenza laterale (Grip)
   // Più la velocità è alta, minore è la capacità di cambiare direzione istantaneamente.
   // I valori 0.05 e 5.0 sono parametri di "Tuning" che potrai regolare nel WorldConfig.h
   double lateralGrip = 5.0 / (std::abs(currSpeed) * 0.05 + 1.0);

   // Interpolazione Lineare (Lerp) per simulare l'inerzia
   // La velocità reale "insegue" la velocità ideale con un ritardo dipendente dal grip
   this->velX += (idealVelX - this->velX) * lateralGrip * deltaTime;
   this->velZ += (idealVelZ - this->velZ) * lateralGrip * deltaTime;

   // 5. Aggiorniamo la posizione nello spazio mondo usando la VERA velocità inerziale
   posX += this->velX * deltaTime;
   posZ += this->velZ * deltaTime;

   if (carModel != nullptr) {
      glm::mat4 newMatrix = glm::mat4(1.0f);
      newMatrix = glm::translate(newMatrix, glm::vec3((float)posX, (float)posY, (float)posZ));
      newMatrix = glm::rotate(newMatrix, (float)carHeadingRad, glm::vec3(0.0f, 1.0f, 0.0f));

      
      carModel->setM(newMatrix);

      // --- INERTIA ---
      // Calcoliamo la vera velocità tangenziale per far ruotare correttamente i modelli 3D
      double actualTangentialSpeed = std::sqrt(velX * velX + velZ * velZ);

      // Se stiamo andando in retromarcia (currSpeed < 0), invertiamo il verso di rotazione
      if (currSpeed < 0) {
         actualTangentialSpeed = -actualTangentialSpeed;
      }

      double distanceMoved = actualTangentialSpeed * deltaTime;

      wheels[0].setSteeringAngle(steeringAngle);
      wheels[1].setSteeringAngle(steeringAngle);
      

      for (int i = 0; i < 4; i++) {
         wheels[i].updateRolling(distanceMoved);
         wheels[i].updateVisuals();
      }
   }
}

void Car::setSteeringAngleDirect(double angle)
{
   if (angle > maxSteeringAngle)  angle = maxSteeringAngle;
   if (angle < -maxSteeringAngle) angle = -maxSteeringAngle;
   steeringAngle = angle;
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

void Car::init(Node* passedNode)
{
   Node* rootScene = passedNode;
   while (rootScene->getParent() != nullptr)
      rootScene = rootScene->getParent(); 

   this->carModel = rootScene->findByName("Car");
   if (!this->carModel) return; 

   glm::mat4 ovoMatrix = this->carModel->getM();

   this->posX = ovoMatrix[3][0];
   this->posY = ovoMatrix[3][1];
   this->posZ = ovoMatrix[3][2];

   // IMPOSTIAMO LA MATRICE INIZIALE DELLA MACCHINA 
   glm::mat4 startMatrix = glm::mat4(1.0f);
   startMatrix = glm::translate(startMatrix, glm::vec3((float)posX, (float) posY, (float)posZ));
   this->carModel->setM(startMatrix);

   //  RECUPERIAMO E AGGANCIAMO LE RUOTE
   for (int i = 0; i < 4; i++) {
      Node* ruota = rootScene->findByName(wheelNames[i]); 
      if (!ruota) continue; 

      // Salviamo la matrice world originaria estratta dall'OVO
      glm::mat4 origRuotaWorld = ruota->getWorldFinalMatrix();

      // Calcoliamo la matrice relativa rispetto alla macchina
      glm::mat4 relRuotaM = glm::inverse(startMatrix) * origRuotaWorld; // 

      // Facciamo il reparenting
      if (ruota->getParent())
         ruota->getParent()->removeChild(ruota);
      this->carModel->addChild(ruota);
      ruota->setM(relRuotaM);

      wheels[i].init(ruota, 1.0f, 0.0, 3.8f, 0.0); 
   }

   wheels[2].setSteeringAngle(0.0);
   wheels[3].setSteeringAngle(0.0);
  
   //DEBUG
   for (int i = 0; i < 4; i++) {
      std::cout << wheels[i].getNode()->getName() << std::endl;
   }
}

void Car::updateSteeringAngle(double const deltaTime) {
   if (isMouseSteering) {
      double diff = mouseSteeringTarget - steeringAngle;
      double step = steeringSpeed * deltaTime;
      if (std::abs(diff) <= step)
         steeringAngle = mouseSteeringTarget;
      else
         steeringAngle += (diff > 0 ? step : -step);
      return; // gli altri casi non servono
   }

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
      if (steeringAngle < 0.0) steeringAngle = 0.0;
   }
   else if (steeringAngle < 0) {
      steeringAngle += steeringSpeed * deltaTime;
      if (steeringAngle > 0.0) steeringAngle = 0.0;
   }
}

glm::mat4 Car::getWorldMatrix() const
{
   if (carModel) return carModel->getWorldFinalMatrix();
   return glm::mat4(1.0f);
}

void Car::setMouseSteeringTarget(double angle) {
   if (angle > maxSteeringAngle) angle = maxSteeringAngle;
   if (angle < -maxSteeringAngle) angle = -maxSteeringAngle;
   mouseSteeringTarget = angle;
}

void Car::setMouseSteering(bool active) {
   isMouseSteering = active;
   if (!active) mouseSteeringTarget = 0.0;
}