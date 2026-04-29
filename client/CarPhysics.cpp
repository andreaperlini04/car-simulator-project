#define _USE_MATH_DEFINES
#include "CarPhysics.h"
#include <cmath>
#include "WorldConfig.h"

CarPhysics::CarPhysics(double maxSpeed,
   double accelerationFactor,
   double brakingFactor,
   double friction,
   double reverseGearMaxSpeed)
   : maxSpeed(maxSpeed),
   accelerationFactor(accelerationFactor),
   brakingFactor(brakingFactor),
   friction(friction),
   reverseGearMaxSpeed(reverseGearMaxSpeed)
{
   // 90.0 allinea l'heading iniziale con l'asse -Z della scena OpenGL
   physicsState.carHeading = 90.0;
}

bool CarPhysics::startEngine() { return isEngineOn = true; }
bool CarPhysics::turnOffEngine() { return isEngineOn = false; }

double CarPhysics::getCurrSpeedAbs() const
{
   return std::abs(physicsState.currSpeed);
}

void CarPhysics::setInitialPosition(double x, double y, double z)
{
   physicsState.posX = x;
   physicsState.posY = y;
   physicsState.posZ = z;
}

void CarPhysics::update(double deltaTime, const CarInputState& input)
{
   // Drop di frame anomali (es. breakpoint o freeze) per prevenire instabilità numerica nell'integrazione
   if (deltaTime < 0.0 || deltaTime > MAX_DELTA_TIME) return;

   applyGas(deltaTime, input);

   double& currSpeed = physicsState.currSpeed;

   if (currSpeed > maxSpeed) currSpeed = maxSpeed;
   if (currSpeed < -reverseGearMaxSpeed) currSpeed = -reverseGearMaxSpeed;

   applyTireScrub(deltaTime, input.steeringAngle);
   applyTurnAndGrip(deltaTime, input);
   applyInertia(deltaTime, input);

   physicsState.posX += physicsState.velX * deltaTime;
   physicsState.posZ += physicsState.velZ * deltaTime;
}

void CarPhysics::applyGas(double deltaTime, const CarInputState& input) {
   double& currSpeed = physicsState.currSpeed;

   if (isEngineOn && input.isAccelerating) {
      currSpeed += accelerationFactor * deltaTime;
   }
   else if (input.isBraking) {
      if (isEngineOn) {
         if (currSpeed > 0) {
            currSpeed -= brakingFactor * deltaTime;
            if (currSpeed < 0) currSpeed = 0;
         }
         else {
            double reverseAccel = accelerationFactor * (reverseGearMaxSpeed / maxSpeed);
            currSpeed -= reverseAccel * deltaTime;
         }
      }
      else {
         if (currSpeed > 0) {
            currSpeed -= brakingFactor * deltaTime;
         }
         else if (currSpeed < 0) {
            currSpeed += brakingFactor * deltaTime;
         }
      }
   }
   else {
      applyFriction(deltaTime);
   }
}

/**
 * @brief Disperde energia cinetica in curva.
 * Simula la resistenza indotta dalla deformazione laterale dello pneumatico proporzionalmente all'angolo di sterzo.
 */
void CarPhysics::applyTireScrub(double deltaTime, double steeringAngle) {
   double& currSpeed = physicsState.currSpeed;
   if (std::abs(steeringAngle) <= MIN_STEERING_THRESHOLD)
      return;

   double scrubFactor = std::abs(steeringAngle) / MAX_STEERING_ANGLE_DEG;
   double speedScrub = scrubFactor * std::abs(currSpeed) * TIRE_SCRUB_FACTOR * deltaTime;

   if (currSpeed > 0)
      currSpeed -= speedScrub;
   else if (currSpeed < 0)
      currSpeed += speedScrub;
}

void CarPhysics::applyTurnAndGrip(double deltaTime, const CarInputState& input) {
   double& currSpeed = physicsState.currSpeed;
   double& carHeading = physicsState.carHeading;
   double steeringAngle = input.steeringAngle;

   // Moltiplicatore attivo se il freno a mano supera la soglia minima di velocità per indurre sovrasterzo
   double currentTurnBoost = (input.isHandbrake && currSpeed > DRIFT_MIN_SPEED) ? driftTurnBoost : 1.0;

   double turnRate = steeringAngle * currSpeed * TURN_RATE_COEFF * currentTurnBoost;

   // Limita l'efficacia dello sterzo ad alte velocità (understeer)
   double grip = friction / (std::abs(currSpeed) * GRIP_SPEED_SCALE + 1.0);
   if (grip > 1.0) grip = 1.0;

   carHeading += turnRate * grip * deltaTime;
}

/**
 * @brief Risolve i vettori di velocità applicando slip angle e aderenza dinamica.
 */
void CarPhysics::applyInertia(double deltaTime, const CarInputState& input) {
   double& currSpeed = physicsState.currSpeed;
   double& carHeading = physicsState.carHeading;

   double carHeadingRad = carHeading * (M_PI / 180.0);
   double forwardX = -std::sin(carHeadingRad);
   double forwardZ = -std::cos(carHeadingRad);

   double idealVelX = forwardX * currSpeed;
   double idealVelZ = forwardZ * currSpeed;

   double lateralGrip = LATERAL_GRIP_BASE / (std::abs(currSpeed) * LATERAL_GRIP_SPEED_SCALE + 1.0);

   if (input.isHandbrake && currSpeed > DRIFT_MIN_SPEED) {
      lateralGrip *= driftGripFactor;

      // Dissipazione cinetica dovuta allo strisciamento trasversale degli pneumatici (sliding)
      double slideDecel = (friction * SLIDE_FRICTION_SCALE + std::abs(currSpeed) * SLIDE_SPEED_SCALE) * deltaTime;
      if (currSpeed > 0) {
         currSpeed -= slideDecel;
         if (currSpeed < 0) currSpeed = 0;
      }
      else {
         currSpeed += slideDecel;
         if (currSpeed > 0) currSpeed = 0;
      }
   }

   // Lerp verso il vettore velocità frontale basato sull'aderenza laterale corrente
   physicsState.velX += (idealVelX - physicsState.velX) * lateralGrip * deltaTime;
   physicsState.velZ += (idealVelZ - physicsState.velZ) * lateralGrip * deltaTime;
}

void CarPhysics::applyFriction(double deltaTime) {
   double& currSpeed = physicsState.currSpeed;

   if (currSpeed > 0) {
      currSpeed -= friction * deltaTime;
      if (currSpeed < 0) currSpeed = 0;
   }
   else if (currSpeed < 0) {
      currSpeed += friction * deltaTime;
      if (currSpeed > 0) currSpeed = 0;
   }
}