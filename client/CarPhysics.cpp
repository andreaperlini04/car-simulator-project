#define _USE_MATH_DEFINES
#include "CarPhysics.h"
#include <cmath>

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
    if (deltaTime < 0.0 || deltaTime > MAX_DELTA_TIME) return;

    applyGas(deltaTime, input);

    double& currSpeed = physicsState.currSpeed;

    // Velocita' cappe
    if (currSpeed > maxSpeed) currSpeed = maxSpeed;
    if (currSpeed < -reverseGearMaxSpeed) currSpeed = -reverseGearMaxSpeed;

    applyTireScrub(deltaTime, input.steeringAngle);
    applyTurnAndGrip(deltaTime, input);
    applyInertia(deltaTime, input);

    // Aggiorna posizione con la velocita' inerziale
    physicsState.posX += physicsState.velX * deltaTime;
    physicsState.posZ += physicsState.velZ * deltaTime;

}


void CarPhysics::applyGas(double deltaTime, const CarInputState& input) {
    double& currSpeed = physicsState.currSpeed;

    // Accelerating 
    if (isEngineOn && input.isAccelerating) {
        currSpeed += accelerationFactor * deltaTime;
    }
    // Braking
    else if (input.isBraking) {
        if (isEngineOn)
        {
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

// Sfregamento delle gomme in curva: se si sterza a forte velocita', la macchina rallenta
void CarPhysics::applyTireScrub(double deltaTime, double steeringAngle) {

    double& currSpeed = physicsState.currSpeed;
    if (std::abs(steeringAngle) <= MIN_STEERING_THRESHOLD) 
        return;

    double scrubFactor = std::abs(steeringAngle) / MAX_STEERING_ANGLE;
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
    double currentTurnBoost;

    // Se c'e' il freno a mano tirato, la macchina gira molto piu' velocemente (sovrasterzo)

    if (input.isHandbrake && currSpeed > DRIFT_MIN_SPEED) {
        currentTurnBoost = driftTurnBoost;
    }
    else {
        currentTurnBoost = 1.0;
    }

    double turnRate = steeringAngle * currSpeed * TURN_RATE_COEFF * currentTurnBoost;

    double grip = friction / (std::abs(currSpeed) * GRIP_SPEED_SCALE + 1.0);
    if (grip > 1.0) grip = 1.0;

    carHeading += turnRate * grip * deltaTime;
}

void CarPhysics::applyInertia(double deltaTime, const CarInputState& input) {

    double& currSpeed = physicsState.currSpeed;
    double& carHeading = physicsState.carHeading;

    // --- INERTIA ---
    double carHeadingRad = carHeading * (M_PI / 180.0);
    double forwardX = -std::sin(carHeadingRad);
    double forwardZ = -std::cos(carHeadingRad);

    // Calcoliamo la velocita ideale
    double idealVelX = forwardX * currSpeed;
    double idealVelZ = forwardZ * currSpeed;

    // Calcoliamo l'aderenza laterale (Grip)
    double lateralGrip = LATERAL_GRIP_BASE / (std::abs(currSpeed) * LATERAL_GRIP_SPEED_SCALE + 1.0);

    // Drift: perdiamo aderenza laterale e decelerazione extra
    if (input.isHandbrake && currSpeed > DRIFT_MIN_SPEED) {
        lateralGrip *= driftGripFactor; // Perdiamo aderenza laterale e iniziamo drifting

        // Slittare causa velocita decrescente
        // Decelerazione costante e proporzionale alla velocita'
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

    // Interpolazione Lineare (Lerp) per simulare l'inerzia
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