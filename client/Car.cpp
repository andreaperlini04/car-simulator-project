#include "Car.h"

Car::Car(double maxSpeed,
    double accelerationFactor,
    double brakingFactor,
    double friction,
    double reverseGearMaxSpeed)
    : physics(maxSpeed, accelerationFactor, brakingFactor, friction, reverseGearMaxSpeed)
{
    // CarInputController e CarRenderer si auto-inizializzano
}

void Car::init(Node* carNode)
{
    double posX = 0.0, posY = 0.0, posZ = 0.0;
    if (renderer.init(carNode, posX, posY, posZ))
        physics.setInitialPosition(posX, posY, posZ);
}

// Engine
bool Car::startEngine() { return physics.startEngine(); }
bool Car::turnOffEngine() { return physics.turnOffEngine(); }
bool Car::isEngineStarted()   const { return physics.isEngineStarted(); }

// Input 
void Car::setAccelerating(bool v) { inputController.setAccelerating(v); }
void Car::setBraking(bool v) { inputController.setBraking(v); }
void Car::setHandbrake(bool v) { inputController.setHandbrake(v); }
void Car::setSteeringLeft(bool v) { inputController.setSteeringLeft(v); }
void Car::setSteeringRight(bool v) { inputController.setSteeringRight(v); }
void Car::setSteeringAngleDirect(double a) { inputController.setSteeringAngleDirect(a); }
void Car::setMouseSteeringTarget(double a) { inputController.setMouseSteeringTarget(a); }
void Car::setMouseSteering(bool active) { inputController.setMouseSteering(active); }

// Frame update 
// Car delega tutto
void Car::update(double deltaTime)
{
    // Input: calcola il nuovo angolo di sterzata
    inputController.updateSteeringAngle(deltaTime);

    // Fisica
    physics.update(deltaTime, inputController.getState());

    // Rendering: 
    renderer.update(physics.getState(), inputController.getState().steeringAngle, deltaTime);
}

// Letture
double    Car::getCurrSpeedAbs() const { return physics.getCurrSpeedAbs(); }
double    Car::getMaxSpeed()     const { return physics.getMaxSpeed(); }
glm::mat4 Car::getWorldMatrix()  const { return renderer.getWorldMatrix(); }