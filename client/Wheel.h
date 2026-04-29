#pragma once
#include "node.h"

/**
 * @brief Gestisce le trasformazioni locali e l'animazione visiva della singola ruota.
 * Isola il nodo dalle deformazioni indotte dalla gerarchia del modello 3D.
 */
class Wheel
{
public:
   Wheel() = default;

   /**
    * @brief Associa la geometria e compensa le scale non uniformi esportate dal DCC.
    * @param wheelWorldScaleY Modulo della colonna Y della matrice world pre-reparenting.
    * Necessario per dedurre il raggio locale reale della mesh.
    */
   void init(Node* model, float wheelWorldScaleY, double offX, double offY, double offZ);

   void setSteeringAngle(double angle);
   void updateRolling(double distanceMoved);

   // Ricalcola la matrice locale combinando baseMatrix, Yaw (sterzo) e Pitch (rotolamento)
   void updateVisuals();

   Node* getNode() const;

private:
   // Raw pointer: ownership gestita dallo scene graph
   Node* wheelModel = nullptr;

   // Snapshot della trasformazione iniziale, base "pulita" per applicare le rotazioni
   glm::mat4 baseMatrix = glm::mat4(1.0f);

   // Dati ricavati dinamicamente dal pivot per evitare hardcoding
   float  m_wheelWorldScaleY = 1.0f; // Fattore di correzione per la gerarchia rotta
   float  m_radius = 0.0f;           // Raggio assoluto (world) dedotto dall'altezza Y
   float  m_localRadius = 0.0f;      // Raggio nello spazio mesh per i radianti di rotolamento

   // Offset dal pivot/centro di massa dell'auto
   double offsetX = 0.0;
   double offsetY = 0.0;
   double offsetZ = 0.0;

   double steeringAngle = 0.0;
   double rollingAngle = 0.0;
};