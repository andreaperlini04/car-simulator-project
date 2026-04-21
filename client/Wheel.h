#pragma once
#include "node.h"

class Wheel
{
public:
   Wheel() = default;

   // wheelWorldScaleY: lunghezza della colonna Y della world matrix della ruota (prima del reparenting)
   void init(Node* model, float wheelWorldScaleY, double offX, double offY, double offZ);

   void setSteeringAngle(double angle);

   void updateRolling(double distanceMoved);

   void updateVisuals();

   Node* getNode() const;

private:
   Node* wheelModel = nullptr;    ///< Observer (non-owning)
   glm::mat4 baseMatrix = glm::mat4(1.0f);

   float  m_wheelWorldScaleY = 1.0f; // scala Y world della ruota (estratta da origRuotaWorld prima del reparenting)
   float  m_radius = 0.0f;           // raggio in unita' mondo (auto-calcolato da pivotWorldY)
   float  m_localRadius = 0.0f;      // raggio in spazio locale mesh (m_radius / m_wheelWorldScaleY)

   // Related to the centre of the car
   double offsetX = 0.0;
   double offsetY = 0.0;
   double offsetZ = 0.0;

   double steeringAngle = 0.0;
   double rollingAngle = 0.0;
};