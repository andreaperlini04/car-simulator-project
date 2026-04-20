#pragma once
#include "CarState.h"
#include "node.h"
#include "wheel.h"


/**
 * @brief Gestisce tutto cio' che riguarda la rappresentazione visiva dell'auto.
 *
 * Responsabilita' uniche:
 *  - Trovare e collegare i nodi 3D della scena (carModel, ruote).
 *  - Aggiornare la matrice mondo del modello in base allo stato fisico.
 *  - Animare le ruote (rotazione, sterzata).
 *  - Non conosce nulla di input ne' di fisica.
 */
class CarRenderer {
public:
  /**
   * @brief Inizializza il renderer cercando i nodi nella scena a partire da
   * passedNode.
   * @param passedNode  Qualsiasi nodo della gerarchia di scena.
   * @param outInitPos  [out] Posizione iniziale letta dalla matrice OVO del
   * modello.
   * @return true se il nodo "Car" e' stato trovato, false altrimenti.
   */
  bool init(Node *passedNode, double &outPosX, double &outPosY,
            double &outPosZ);

  /**
   * @brief Aggiorna la matrice del modello e l'animazione delle ruote.
   * @param physics       Snapshot prodotto da CarPhysics.
   * @param steeringAngle Angolo corrente di sterzata (da CarInputController).
   */
  void update(const CarPhysicsState &physics, double steeringAngle,
              double deltaTime);

  glm::mat4 getWorldMatrix() const;

  bool isReady() const { return carModel != nullptr; }

private:
  Node *carModel = nullptr;
  Wheel wheels[4];

  static constexpr const char *WHEEL_NAMES[4] = {"RuotaAD", "RuotaAS",
                                                 "RuotaPD", "RuotaPS"};

  // Costanti di inizializzazione ruote
  static constexpr float WHEEL_RADIUS = 1.0f; // Wheel radius (world units)
  static constexpr double WHEEL_OFFSET_X =
      0.0; // Wheel X offset relative to hub
  static constexpr float WHEEL_OFFSET_Y =
      3.8f; // Wheel Y offset relative to hub (rim height)
  static constexpr double WHEEL_OFFSET_Z =
      0.0; // Wheel Z offset relative to hub
};