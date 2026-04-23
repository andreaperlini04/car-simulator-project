#pragma once
#include "CarState.h"
#include "node.h"
#include "wheel.h"

/**
 * @brief Sincronizza il transform del modello 3D e l'animazione delle ruote con lo stato simulato.
 * Mantiene lo scene graph completamente isolato dai calcoli della fisica pura.
 */
class CarRenderer {
public:
   /**
    * @brief Aggancia la geometria dell'auto e ne estrae la trasformazione di base.
    * Attraversa l'albero per trovare il nodo "Car" e bindare i figli delle ruote.
    * @param passedNode Nodo di partenza per la ricerca (es. root della scena).
    * @param outPosX, outPosY, outPosZ Coordinate world estratte per inizializzare il solver fisico.
    * @return true se il parsing della gerarchia va a buon fine.
    */
   bool init(Node* passedNode, double& outPosX, double& outPosY,
      double& outPosZ);

   /**
    * @brief Ricalcola le matrici locali e applica le rotazioni cinematiche.
    * @param deltaTime Integrato per calcolare i radianti di rotazione visiva delle ruote.
    */
   void update(const CarPhysicsState& physics, double steeringAngle,
      double deltaTime);

   glm::mat4 getWorldMatrix() const;

   bool isReady() const { return carModel != nullptr; }

private:
   Node* carModel = nullptr;
   Wheel wheels[4];

   // Mapping string per agganciare correttamente i nodi esportati da 3ds Max
   static constexpr const char* WHEEL_NAMES[4] = { "RuotaAD", "RuotaAS",
                                                  "RuotaPD", "RuotaPS" };

   // Compensazioni per allineare il raycast fisico al pivot visivo del modello OVO
   static constexpr float WHEEL_RADIUS = 1.0f;
   static constexpr double WHEEL_OFFSET_X = 0.0;
   static constexpr float WHEEL_OFFSET_Y = 3.8f;
   static constexpr double WHEEL_OFFSET_Z = 0.0;
};