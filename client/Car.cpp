#include "Car.h"
#include "engine.h"
#include <iostream>

Car::Car()
{
   this->isEngineOn = false;
   this->isAccelerating = false;
   this->isBraking = false;

   this->maxSpeed = 25.0;
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

void Car::setAccelerating(bool v) { this->isAccelerating = v; }
void Car::setBraking(bool v) { this->isBraking = v; }
void Car::setSteering(double angle) { this->steeringAngle = angle; }

void Car::update(double deltaTime)
{
   if (deltaTime < 0) return;

   // -------  VELOCITA'  -------
   if (isAccelerating) {
      currSpeed += accelerationFactor * deltaTime;
   }
   else if (isBraking) {
      currSpeed -= brakingFactor * deltaTime;
   }
   else {
      // Attrito terreno
      if (currSpeed > 0) {
         currSpeed -= friction * deltaTime;
         if (currSpeed < 0) currSpeed = 0;
      }
      else if (currSpeed < 0) {
         currSpeed += friction * deltaTime;
         if (currSpeed > 0) currSpeed = 0;
      }
   }
   if (currSpeed > maxSpeed) currSpeed = maxSpeed;
   if (currSpeed < -maxSpeed) currSpeed = -maxSpeed;

   // -------  STERZO  -------
   if (currSpeed != 0) {
      double turnRate = steeringAngle * currSpeed * 0.05;
      double grip = friction / (std::abs(currSpeed) * 0.25 + 1.0);
      if (grip > 1.0) grip = 1.0;
      carHeading += turnRate * grip * deltaTime;
   }

   // -------  POSIZIONE  -------
   double carHeadingRad = carHeading * (3.14159265358979 / 180.0);
   posX += currSpeed * std::sin(carHeadingRad) * deltaTime;
   posZ += currSpeed * std::cos(carHeadingRad) * deltaTime;

   // -------  VISUAL UPDATE  -------
   if (carModel != nullptr) {
      glm::mat4 newMatrix = glm::mat4(1.0f);
      newMatrix = glm::translate(newMatrix, glm::vec3((float)posX, originalY, (float)posZ));
      newMatrix = glm::rotate(newMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      newMatrix = glm::rotate(newMatrix, (float)carHeadingRad, glm::vec3(0.0f, 1.0f, 0.0f));
      newMatrix = glm::scale(newMatrix, originalScale);
      carModel->setM(newMatrix);

      double distanceMoved = currSpeed * deltaTime;

      // Sterzo solo ruote anteriori (0=RuotaAD, 1=RuotaAS)
      wheels[0].setSteeringAngle(steeringAngle);
      wheels[1].setSteeringAngle(steeringAngle);
      // Ruote posteriori dritte
      wheels[2].setSteeringAngle(0.0);
      wheels[3].setSteeringAngle(0.0);

      for (int i = 0; i < 4; i++) {
         wheels[i].updateRolling(distanceMoved);
         wheels[i].updateVisuals();
      }
   }
}

void Car::init(Node* passedNode, int startX, int startZ)
{
   this->posX = startX;
   this->posZ = startZ;

   // Risali fino alla radice della scena
   Node* rootScene = passedNode;
   while (rootScene->getParent() != nullptr)
      rootScene = rootScene->getParent();

   std::cout << "Init Car: radice='" << rootScene->getName() << "'" << std::endl;

   this->carModel = rootScene->findByName("Car");
   if (!this->carModel) {
      std::cout << "ERRORE CRITICO: nodo 'Car' non trovato!" << std::endl;
      return;
   }

   // -------  Scala e posizione iniziale della carrozzeria  -------
   glm::mat4 localMatrix = this->carModel->getM();
   this->originalScale = glm::vec3(
      glm::length(glm::vec3(localMatrix[0])),
      glm::length(glm::vec3(localMatrix[1])),
      glm::length(glm::vec3(localMatrix[2]))
   );

   glm::mat4 worldMatrix = this->carModel->getWorldFinalMatrix();
   this->originalY = worldMatrix[3][1];

   glm::vec3 forwardVec = glm::normalize(glm::vec3(
      worldMatrix[2][0] / originalScale.x,
      worldMatrix[2][1] / originalScale.y,
      worldMatrix[2][2] / originalScale.z
   ));
   float initHeadingRad = atan2f(forwardVec.x, forwardVec.z);
   this->carHeading = (double)(initHeadingRad * 180.0 / 3.14159265358979);
   this->posX = worldMatrix[3][0];
   this->posZ = worldMatrix[3][2];

   glm::mat4 cleanCarMatrix = glm::mat4(1.0f);
   cleanCarMatrix = glm::translate(cleanCarMatrix, glm::vec3((float)posX, originalY, (float)posZ));
   cleanCarMatrix = glm::rotate(cleanCarMatrix, initHeadingRad, glm::vec3(0.0f, 1.0f, 0.0f));
   cleanCarMatrix = glm::scale(cleanCarMatrix, originalScale);

   std::cout << " -> Scale: (" << originalScale.x << ", " << originalScale.y << ", " << originalScale.z
      << ")  Y: " << originalY
      << "  Heading: " << this->carHeading << " deg" << std::endl;

   // -------  Collega le ruote alla carrozzeria  -------
   for (int i = 0; i < 4; i++) {
      Node* ruota = rootScene->findByName(wheelNames[i]);
      Node* cerchione = rootScene->findByName(rimNames[i]);

      if (!ruota) {
         std::cout << "ATTENZIONE: ruota '" << wheelNames[i] << "' non trovata!" << std::endl;
         continue;
      }

      // Salva la world matrix PRIMA del reparenting (necessaria per calcoli successivi)
      glm::mat4 origRuotaWorld = ruota->getWorldFinalMatrix();

      // -- scala world Y della ruota (PRIMA del reparenting) --
      // Usata da Wheel::init per convertire il raggio dal mondo allo spazio locale
      float wheelWorldScaleY = glm::length(glm::vec3(origRuotaWorld[1]));

      // -- posizione relativa ruota rispetto alla carrozzeria --
      glm::mat4 relRuotaM = glm::inverse(worldMatrix) * origRuotaWorld;

      // Reparenting: stacca dal padre originale, agganciala alla carrozzeria
      if (ruota->getParent())
         ruota->getParent()->removeChild(ruota);
      this->carModel->addChild(ruota);
      ruota->setM(relRuotaM);

      // Inizializza la logica della ruota
      // wheelWorldScaleY permette di convertire correttamente il raggio mondo -> locale
      wheels[i].init(ruota, wheelWorldScaleY, 0.0, 0.0, 0.0);
      std::cout << "    -> Collegata: " << wheelNames[i]
         << "  worldScaleY=" << wheelWorldScaleY << std::endl;

      // -- Cerchione: figlio della ruota --
      if (cerchione) {
         glm::mat4 relCerchioneM = glm::inverse(origRuotaWorld) * cerchione->getWorldFinalMatrix();
         if (cerchione->getParent())
            cerchione->getParent()->removeChild(cerchione);
         ruota->addChild(cerchione);
         cerchione->setM(relCerchioneM);
         std::cout << "    -> Cerchione: " << rimNames[i]
            << " (figlio di " << wheelNames[i] << ")" << std::endl;
      }
   }

   // Applica la matrice pulita alla carrozzeria (sovrascrive quella dell'OVO con una consistente)
   this->carModel->setM(cleanCarMatrix);
}

glm::mat4 Car::getWorldMatrix() const
{
   if (carModel) return carModel->getWorldFinalMatrix();
   return glm::mat4(1.0f);
}