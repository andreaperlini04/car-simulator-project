#define _USE_MATH_DEFINES
#include "CarRenderer.h"
#include "engine.h"
#include <iostream>
#include <cmath>

/**
 * @brief Inizializza il modello visivo e mappa la gerarchia delle ruote.
 * @param passedNode Nodo di partenza per la ricerca nello scene graph.
 * @param outPosX, outPosY, outPosZ Coordinate assolute estratte dal file OVO.
 */
bool CarRenderer::init(Node* passedNode, double& outPosX, double& outPosY, double& outPosZ) {

   // Risale al root per garantire la ricerca globale dei nodi della vettura
   Node* rootScene = passedNode;
   while (rootScene->getParent() != nullptr)
      rootScene = rootScene->getParent();

   this->carModel = rootScene->findByName("Car");
   if (!this->carModel) return false;

   glm::mat4 ovoMatrix = this->carModel->getM();

   outPosX = ovoMatrix[3][0];
   outPosY = ovoMatrix[3][1];
   outPosZ = ovoMatrix[3][2];

   glm::mat4 startMatrix = glm::mat4(1.0f);
   startMatrix = glm::translate(startMatrix, glm::vec3((float)outPosX, (float)outPosY, (float)outPosZ));
   this->carModel->setM(startMatrix);

   for (int i = 0; i < 4; i++) {
      Node* ruota = rootScene->findByName(WHEEL_NAMES[i]);
      if (!ruota) continue;

      // Conserva la trasformazione assoluta dell'OVO per ricavare l'offset locale rispetto allo chassis
      glm::mat4 origRuotaWorld = ruota->getWorldFinalMatrix();
      glm::mat4 relRuotaM = glm::inverse(startMatrix) * origRuotaWorld;

      // Trasferisce la ownership delle ruote sotto 'Car' per propagare le trasformazioni parent-child
      if (ruota->getParent()) {
         Node* extractedWheel = ruota->getParent()->removeChild(ruota);
         this->carModel->addChild(extractedWheel);
      }
      ruota->setM(relRuotaM);

      wheels[i].init(ruota);
   }

   // Blocca l'angolo di sterzo delle ruote posteriori (trazione anteriore o integrale base)
   wheels[2].setSteeringAngle(0.0);
   wheels[3].setSteeringAngle(0.0);

   for (int i = 0; i < 4; i++) {
      std::cout << wheels[i].getNode()->getName() << std::endl;
   }
   return true;
}

/**
 * @brief Sincronizza la matrice del modello 3D con lo stato fisico corrente.
 */
void CarRenderer::update(const CarPhysicsState& physics, double steeringAngle, double deltaTime) {
   if (this->carModel != nullptr) {
      glm::mat4 newMatrix = glm::mat4(1.0f);

      double carHeadingRad = physics.carHeading * (M_PI / 180.0);

      newMatrix = glm::translate(newMatrix, glm::vec3((float)physics.posX, (float)physics.posY, (float)physics.posZ));
      newMatrix = glm::rotate(newMatrix, (float)carHeadingRad, glm::vec3(0.0f, 1.0f, 0.0f));

      carModel->setM(newMatrix);

      // Modulo della velocità sul piano XZ per animare il rotolamento in modo coerente con derive e drift
      double actualTangentialSpeed = std::sqrt(physics.velX * physics.velX + physics.velZ * physics.velZ);

      if (physics.currSpeed < 0) {
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

glm::mat4 CarRenderer::getWorldMatrix() const {
   if (carModel)
      return carModel->getWorldFinalMatrix();
   return glm::mat4(1.0f);
}