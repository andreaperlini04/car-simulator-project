#include "wheel.h"
#include <iostream>
#include <cmath>

Wheel::Wheel()
{
   wheelModel = nullptr;
   m_wheelWorldScaleY = 1.0f;
   m_radius = 0.0f;
   m_localRadius = 0.0f;

   offsetX = 0.0;
   offsetY = 0.0;
   offsetZ = 0.0;

   steeringAngle = 0.0;
   oldSteeringAngle = 0.0;
   rollingAngle = 0.0;
}

void Wheel::init(Node* model, float wheelWorldScaleY, double offX, double offY, double offZ)
{
   wheelModel = model;
   offsetX = offX;
   offsetY = offY;
   offsetZ = offZ;

   

   if (wheelModel) {
      baseMatrix = wheelModel->getM();
      glm::mat4 worldM = wheelModel->getWorldFinalMatrix();

      float pivotWorldY = worldM[3][1];
      m_radius = pivotWorldY;

  
   
   }
}

void Wheel::setSteeringAngle(double angle)
{
   oldSteeringAngle = steeringAngle;
   steeringAngle = angle;
}

void Wheel::updateRolling(double distanceMoved)
{
   if (m_radius <= 0.0f) return;

   // Angolo di rotolamento in gradi = (distanza / raggio) * (180 / PI)
   double angleDeg = (distanceMoved / m_radius) * (180.0 / 3.14159265358979);
   rollingAngle += angleDeg;

   if (rollingAngle > 360.0) rollingAngle -= 360.0;
   if (rollingAngle < -360.0) rollingAngle += 360.0;
}

void Wheel::updateVisuals()
{
   if (!wheelModel) return;

   
   glm::mat4 m = baseMatrix;


 
   m = glm::rotate(m, glm::radians((float)steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));

   
   m = glm::rotate(m, glm::radians(-(float)rollingAngle), glm::vec3(1.0f, 0.0f, 0.0f));

   wheelModel->setM(m);
}