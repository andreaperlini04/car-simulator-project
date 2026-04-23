#include "wheel.h"
#include <iostream>
#include <cmath>

Node* Wheel::getNode() const {
   return wheelModel;
}

/**
 * @brief Inizializza la ruota calcolando dinamicamente il raggio dalla trasformazione globale.
 */
void Wheel::init(Node* model, float wheelWorldScaleY, double offX, double offY, double offZ)
{
   wheelModel = model;
   offsetX = offX;
   offsetY = offY;
   offsetZ = offZ;

   if (wheelModel) {
      baseMatrix = wheelModel->getM();
      glm::mat4 worldM = wheelModel->getWorldFinalMatrix();

      // Assunzione critica sul file OVO: la mesh della ruota deve poggiare esattamente
      // sul piano globale Y=0. Cosi' l'altezza del pivot corrisponde fisicamente al raggio.
      float pivotWorldY = worldM[3][1];
      m_radius = pivotWorldY;
   }
}

void Wheel::setSteeringAngle(double angle)
{
   steeringAngle = angle;
}

void Wheel::updateRolling(double distanceMoved)
{
   if (m_radius <= 0.0f) return;

   float angleDeg = (distanceMoved / m_radius) * (180.0 / glm::pi<float>());
   rollingAngle += angleDeg;

   // Wrap per prevenire instabilità e perdita di precisione float su istanze in esecuzione da molto tempo
   if (rollingAngle > 360.0) rollingAngle -= 360.0;
   if (rollingAngle < -360.0) rollingAngle += 360.0;
}

void Wheel::updateVisuals()
{
   if (!wheelModel) return;

   glm::mat4 m = baseMatrix;

   // L'ordine delle rotazioni è tassativo: lo sterzo (asse Y) ridefinisce il frame locale
   // prima di applicare il rotolamento (asse X). Invertirli causerebbe la rotazione
   // della ruota su un asse globale errato durante le curve.
   m = glm::rotate(m, glm::radians((float)steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));
   m = glm::rotate(m, glm::radians(-(float)rollingAngle), glm::vec3(1.0f, 0.0f, 0.0f));

   wheelModel->setM(m);
}