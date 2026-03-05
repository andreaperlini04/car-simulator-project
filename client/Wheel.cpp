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

   m_wheelWorldScaleY = (wheelWorldScaleY > 0.000001f) ? wheelWorldScaleY : 1.0f;

   if (wheelModel) {
      baseMatrix = wheelModel->getM();

      // Il pivot nell'OVO e' in basso (Y=0 locale = fondo ruota).
      // La world Y del nodo corrisponde alla posizione del FONDO della ruota nel mondo.
      // Assumendo suolo a Y=0: m_radius (in unita' mondo) = pivot world Y.
      //
      // Se il risultato non e' perfetto, puoi sostituire questa riga con
      // un valore misurato in 3DS Max, ad esempio:
      //   m_radius = 3.534f / 2.0f;
      glm::mat4 worldM = wheelModel->getWorldFinalMatrix();
      float pivotWorldY = worldM[3][1]; // Y mondo del pivot (fondo ruota)
      m_radius = 3.534f / 2.0f;         // diametro di 3dsmax

      // Converte il raggio in spazio locale della mesh
      // (divide per la scala world della ruota)
      m_localRadius = m_radius / m_wheelWorldScaleY;

      std::cout << "[Wheel] init: pivotWorldY=" << pivotWorldY
         << "  worldScaleY=" << m_wheelWorldScaleY
         << "  m_radius=" << m_radius
         << "  m_localRadius=" << m_localRadius << std::endl;
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

   // Partiamo dalla matrice locale originale della ruota (posizione relativa alla macchina)
   glm::mat4 m = baseMatrix;

   // Il pivot nell'OVO e' in basso (Y=0 locale = fondo ruota).
   // SANDWICH TRICK: sposta il pivot al centro, ruota, riporta al punto originale.
   //
   //   T(+localRadius)  -->  porta il centro geometrico della ruota all'origine locale
   //   Rotazione        -->  ruota attorno all'origine (= centro ruota) ✓
   //   T(-localRadius)  -->  riporta tutto nella posizione originale
   //
   m = glm::translate(m, glm::vec3(0.0f, m_localRadius, 0.0f));

   // Sterzo (solo ruote anteriori, impostato da Car::update)
   m = glm::rotate(m, glm::radians((float)steeringAngle), glm::vec3(0.0f, 1.0f, 0.0f));

   // Rotolamento
   m = glm::rotate(m, glm::radians(-(float)rollingAngle), glm::vec3(1.0f, 0.0f, 0.0f));

   m = glm::translate(m, glm::vec3(0.0f, -m_localRadius, 0.0f));

   wheelModel->setM(m);
}