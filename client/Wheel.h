#pragma once
#include "node.h"

class Wheel
{
public:
   Wheel();

   // wheelWorldScaleY: lunghezza della colonna Y della world matrix della ruota (prima del reparenting)
   void init(Node* model, float wheelWorldScaleY, double offX, double offY, double offZ);

   void setSteeringAngle(double angle);

   void updateRolling(double distanceMoved);

   void updateVisuals();

   Node* getNode();

private:
   Node* wheelModel;
   glm::mat4 baseMatrix;

   float  m_wheelWorldScaleY; // scala Y world della ruota (estratta da origRuotaWorld prima del reparenting)
   float  m_radius;           // raggio in unita' mondo (auto-calcolato da pivotWorldY)
   float  m_localRadius;      // raggio in spazio locale mesh (m_radius / m_wheelWorldScaleY)

   // Related to the centre of the car
   double offsetX;
   double offsetY;
   double offsetZ;

   double steeringAngle;
   double oldSteeringAngle;
   double rollingAngle;
};