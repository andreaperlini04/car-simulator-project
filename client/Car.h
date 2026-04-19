#pragma once
#include "node.h"
#include "wheel.h"

class Car
{
public:
   Car(double maxSpeed, double accelerationFactor, double brakingFactor, double friction, double reverseGearMaxSpeed);

   void init(Node* carNode);

   bool startEngine();
   bool turnOffEngine();

   void setAccelerating(bool isAccelerating);
   void setBraking(bool isBraking);
   void setHandbrake(bool isHandbrake);
   void setSteeringLeft(bool isSteeringLeft);
   void setSteeringRight(bool isSteeringRight);
   bool isEngineStarted() const;

   void update(double deltaTime);

   double getCurrSpeedAbs() const;

   double getMaxSpeed() const;


   void setSteeringAngleDirect(double angle);
   void setMouseSteeringTarget(double angle);
   void setMouseSteering(bool active);



   glm::mat4 getWorldMatrix() const;
   
   const std::string wheelNames[4] = { "RuotaAD", "RuotaAS", "RuotaPD", "RuotaPS" };

private:
   Node* carModel = nullptr;
   Wheel  wheels[4];

   double maxSpeed;
   double currSpeed;
   // --- Vettori per l'inerzia ---
   double velX = 0.0;
   double velZ = 0.0;
   double accelerationFactor;
   double brakingFactor;
   double friction;
   double reverseGearMaxSpeed;

   double posX;
   double posY;
   double posZ;

   double carHeading = 90.0;
   double steeringAngle = 0.0; // Positive angle => going left
   const double maxSteeringAngle = 35.0; // Degrees 

   bool isSteeringLeft = false;
   bool isSteeringRight = false;

   bool isEngineOn = false;
   bool isAccelerating = false;
   bool isBraking = false;
   bool isHandbrake = false;

   // Drift parameters
   double driftGripFactor = 0.15; // Less lateral grip when handbrake is activated
   double driftTurnBoost = 2.5;   // How fast the car rotates while drifting
   double driftSpeedDecay = 3.0;  // Extra friction applied while sliding

   const double steeringSpeed = 50.0; // Degree/s

   // Physics constants
   static constexpr double MAX_DELTA_TIME = 1.0;         // Maximum allowed deltaTime (s)
   static constexpr double MIN_STEERING_THRESHOLD = 0.1; // Min steering angle (deg) for tire scrub
   static constexpr double TIRE_SCRUB_FACTOR = 0.45;     // Speed loss coefficient during cornering
   static constexpr double DRIFT_MIN_SPEED = 5.0;        // Minimum speed (units/s) to trigger drift
   static constexpr double TURN_RATE_COEFF = 0.020;      // Turn rate coefficient (rad / (deg * unit/s))
   static constexpr double GRIP_SPEED_SCALE = 0.25;      // Speed scaling factor in grip falloff
   static constexpr double LATERAL_GRIP_BASE = 18.0;     // Base lateral grip multiplier
   static constexpr double LATERAL_GRIP_SPEED_SCALE = 0.02; // Speed scaling factor in lateral grip falloff
   static constexpr double SLIDE_FRICTION_SCALE = 10.0;  // Friction multiplier for slide deceleration
   static constexpr double SLIDE_SPEED_SCALE = 0.3;      // Speed-proportional component of slide decel

   // Wheel initialisation constants
   static constexpr float  WHEEL_RADIUS = 1.0f;    // Wheel radius (world units)
   static constexpr double WHEEL_OFFSET_X = 0.0;   // Wheel X offset relative to hub
   static constexpr float  WHEEL_OFFSET_Y = 3.8f;  // Wheel Y offset relative to hub (rim height)
   static constexpr double WHEEL_OFFSET_Z = 0.0;   // Wheel Z offset relative to hub

   void updateSteeringAngle(double deltaTime);

   void applyFriction(double deltaTime);
   bool isMouseSteering = false;
   double mouseSteeringTarget = 0.0;
};