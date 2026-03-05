#include "wheel.h"

Wheel::Wheel()
{
    wheelModel = nullptr;

    // altezza ruota 
    radius = 3.534f / 2.0f;

    offsetX = 0.0;
    offsetY = 0.0;
    offsetZ = 0.0;

    steeringAngle = 0.0;
    oldSteeringAngle = steeringAngle;
    rollingAngle = 0.0;
}


void Wheel::init(Node* model, double wheelRadius, double offX, double offY, double offZ)
{
    wheelModel = model;
    radius = wheelRadius;
    offsetX = offX;
    offsetY = offY;
    offsetZ = offZ;

    if (wheelModel) {
       baseMatrix = wheelModel->getM();
    }
}

void Wheel::setSteeringAngle(double angle)
{
    oldSteeringAngle = steeringAngle;
    steeringAngle = angle;
}

void Wheel::updateRolling(double distanceMoved)
{

   if (radius <= 0.0) return;

   // Angolo (in radianti) = distanza / raggio
   // Moltiplichiamo per (180 / PI) per convertirlo in gradi
   double angleDeg = (distanceMoved / radius) * (180.0 / 3.14159);

   rollingAngle += angleDeg;

   if (rollingAngle > 360.0) rollingAngle -= 360.0;
   if (rollingAngle < -360.0) rollingAngle += 360.0;
}

void Wheel::updateVisuals()
{
   if (!wheelModel) return;

   // scaleY = 0.000834 (non zero, stampava 0.0 per arrotondamento)
   float scaleY = glm::length(glm::vec3(baseMatrix[1]));

   // radius è in unità mondo -> converti in spazio locale della ruota
   // 1.767 / 0.000834 = 2119 unità locali -> proiettato torna 1.767 mondo 
   float localRadius = (scaleY > 0.000001f) ? ((float)radius / scaleY) : 0.0f;

   glm::mat4 m = baseMatrix;

   // Sandwich: porta il pivot al centro, ruota, riporta indietro
   m = glm::translate(m, glm::vec3(0.0f, 221.0f, 0.0f));
   m = glm::rotate(m, glm::radians((float)steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));
   m = glm::rotate(m, glm::radians(-(float)rollingAngle), glm::vec3(1.0f, 0.0f, 0.0f));
   m = glm::translate(m, glm::vec3(0.0f, -221.0f, 0.0f));

   wheelModel->setM(m);
}

