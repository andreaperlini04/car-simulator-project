#pragma once
#include "CarState.h"

/**
 * @brief Modello cinematico/dinamico ibrido del veicolo.
 * Isola l'integrazione di forze, inerzia e drift.
 * Totalmente agnostico rispetto al rendering e al polling degli input.
 */
class CarPhysics {
public:
   CarPhysics(double maxSpeed, double accelerationFactor, double brakingFactor,
      double friction, double reverseGearMaxSpeed);

   bool startEngine();
   bool turnOffEngine();
   bool isEngineStarted() const { return isEngineOn; }

   /**
    * @brief Esegue uno step di integrazione (consigliato Euler semi-implicito a dt fisso).
    * @param deltaTime Step temporale (clamped internamente per stabilità).
    * @param input Snapshot dei comandi attivi in questo tick.
    */
   void update(double deltaTime, const CarInputState& input);

   const CarPhysicsState& getState() const { return physicsState; }
   double getCurrSpeedAbs() const;
   double getMaxSpeed() const { return maxSpeed; }

   // Forza il posizionamento assoluto (es. spawn point) resettando l'inerzia
   void setInitialPosition(double x, double y, double z);

private:
   CarPhysicsState physicsState;

   bool isEngineOn = false;
   double maxSpeed;
   double accelerationFactor;
   double brakingFactor;
   double friction;
   double reverseGearMaxSpeed;

   // Modificatori per gestione slip angle e sbandata
   double driftGripFactor = 0.15;  // Taglio netto al grip laterale col freno a mano
   double driftTurnBoost = 1.7;    // Moltiplicatore di yaw rate per facilitare il controsterzo
   double driftSpeedDecay = 3.0;   // Attrito viscoso extra per dissipare energia in slide

   // Limite al delta per prevenire esplosioni numeriche nel solver
   static constexpr double MAX_DELTA_TIME = 1.0;

   // Deadzone per prevenire micro-attriti a volante dritto
   static constexpr double MIN_STEERING_THRESHOLD = 0.1;

   // Dissipazione energia cinetica in curva (simula la deformazione dello pneumatico)
   static constexpr double TIRE_SCRUB_FACTOR = 0.45;
   static constexpr double DRIFT_MIN_SPEED = 5.0;

   // Relazione lineare angolo/velocità per calcolare il raggio di curvatura
   static constexpr double TURN_RATE_COEFF = 0.020;

   // Smorzamento del grip ad alte velocità (evita ribaltamenti irreali)
   static constexpr double GRIP_SPEED_SCALE = 0.25;
   static constexpr double LATERAL_GRIP_BASE = 18.0;
   static constexpr double LATERAL_GRIP_SPEED_SCALE = 0.04;

   static constexpr double SLIDE_FRICTION_SCALE = 10.0;
   static constexpr double SLIDE_SPEED_SCALE = 0.3;

   // Pipeline di integrazione sequenziale
   void applyGas(double deltaTime, const CarInputState& input);
   void applyTireScrub(double deltaTime, double steeringAngle);
   void applyTurnAndGrip(double deltaTime, const CarInputState& input);
   void applyInertia(double deltaTime, const CarInputState& input);
   void applyFriction(double deltaTime);
};