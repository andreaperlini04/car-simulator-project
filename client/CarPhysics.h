#pragma once
#include "CarState.h"

/**
 * @brief Simula la fisica dell'automobile (velocita', posizione, inerzia,
 * drift).
 *
 * Responsabilita':
 *  - Integrare accelerazione, frenata, attrito, sterzo e inerzia laterale.
 *  - Produrre uno snapshot CarPhysicsState da passare a CarRenderer.
 *  - Non conosce nulla di input diretto ne' di grafica.
 *
 */
class CarPhysics {
public:
  CarPhysics(double maxSpeed, double accelerationFactor, double brakingFactor,
             double friction, double reverseGearMaxSpeed);

  // --- Motore ---
  bool startEngine();
  bool turnOffEngine();
  bool isEngineStarted() const { return isEngineOn; }

  // Aggiornamento frame
  /**
   * @param deltaTime  Secondi dall'ultimo frame.
   * @param input      Snapshot prodotto da CarInputController.
   */
  void update(double deltaTime, const CarInputState &input);

  // Lettura Stato
  const CarPhysicsState &getState() const { return physicsState; }
  double getCurrSpeedAbs() const;
  double getMaxSpeed() const { return maxSpeed; }

  // Posizione iniziale letta dalla scena (usata da CarRenderer::init)
  void setInitialPosition(double x, double y, double z);

private:
  CarPhysicsState physicsState;

  bool isEngineOn = false;
  double maxSpeed;
  double accelerationFactor;
  double brakingFactor;
  double friction;
  double reverseGearMaxSpeed;

  // Parametri drift
  double driftGripFactor =
      0.15;                     // Less lateral grip when handbrake is activated
  double driftTurnBoost = 2.5;  // How fast the car rotates while drifting
  double driftSpeedDecay = 3.0; // Extra friction applied while sliding

  // Costanti fisiche
  static constexpr double MAX_DELTA_TIME = 1.0; // Maximum allowed deltaTime (s)
  static constexpr double MIN_STEERING_THRESHOLD =
      0.1; // Min steering angle (deg) for tire scrub
  static constexpr double TIRE_SCRUB_FACTOR =
      0.45; // Speed loss coefficient during cornering
  static constexpr double DRIFT_MIN_SPEED =
      5.0; // Minimum speed (units/s) to trigger drift
  static constexpr double TURN_RATE_COEFF =
      0.020; // Turn rate coefficient (rad / (deg * unit/s))
  static constexpr double GRIP_SPEED_SCALE =
      0.25; // Speed scaling factor in grip falloff
  static constexpr double LATERAL_GRIP_BASE =
      18.0; // Base lateral grip multiplier
  static constexpr double LATERAL_GRIP_SPEED_SCALE =
      0.02; // Speed scaling factor in lateral grip falloff
  static constexpr double SLIDE_FRICTION_SCALE =
      10.0; // Friction multiplier for slide deceleration
  static constexpr double SLIDE_SPEED_SCALE =
      0.3; // Speed-proportional component of slide decel;
  static constexpr double MAX_STEERING_ANGLE = 35.0; // Gradi

  void applyGas(double deltaTime, const CarInputState &input);
  void applyTireScrub(double deltaTime, double steeringAngle);
  void applyTurnAndGrip(double deltaTime, const CarInputState &input);
  void applyInertia(double deltaTime, const CarInputState &input);
  void applyFriction(double deltaTime);
};