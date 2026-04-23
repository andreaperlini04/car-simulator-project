#include "Car.h"

Car::Car(double maxSpeed,
   double accelerationFactor,
   double brakingFactor,
   double friction,
   double reverseGearMaxSpeed)
   : physics(maxSpeed, accelerationFactor, brakingFactor, friction, reverseGearMaxSpeed)
{
}

void Car::init(Node* carNode)
{
   double posX = 0.0, posY = 0.0, posZ = 0.0;
   if (renderer.init(carNode, posX, posY, posZ))
      physics.setInitialPosition(posX, posY, posZ);
}

bool Car::startEngine() { return physics.startEngine(); }
bool Car::turnOffEngine() { return physics.turnOffEngine(); }
bool Car::isEngineStarted()   const { return physics.isEngineStarted(); }

void Car::setAccelerating(bool v) { inputController.setAccelerating(v); }
void Car::setBraking(bool v) { inputController.setBraking(v); }
void Car::setHandbrake(bool v) { inputController.setHandbrake(v); }
void Car::setSteeringLeft(bool v) { inputController.setSteeringLeft(v); }
void Car::setSteeringRight(bool v) { inputController.setSteeringRight(v); }
void Car::setSteeringAngleDirect(double a) { inputController.setSteeringAngleDirect(a); }
void Car::setMouseSteeringTarget(double a) { inputController.setMouseSteeringTarget(a); }
void Car::setMouseSteering(bool active) { inputController.setMouseSteering(active); }

/**
 * @brief Esegue l'integrazione fisica.
 * Richiede un delta-time fisso per garantire il determinismo della simulazione.
 */
void Car::stepPhysics(double fixedDt)
{
   // Svincola la reattività dell'input dal framerate di rendering
   inputController.updateSteeringAngle(fixedDt);
   physics.update(fixedDt, inputController.getState());
}

/**
 * @brief Sincronizza lo scene graph visivo con l'ultimo stato fisico calcolato.
 */
void Car::updateRenderer(double frameDt)
{
   // frameDt è variabile: usarlo esclusivamente per interpolazioni puramente estetiche (es. rotazione mesh ruote).
   // TODO: Per azzerare eventuale micro-stuttering, valutare il passaggio di un 'alpha' (accumulator / FIXED_DT) per interpolare lo stato fisico precedente e attuale.
   renderer.update(physics.getState(), inputController.getState().steeringAngle, frameDt);
}

double    Car::getCurrSpeedAbs() const { return physics.getCurrSpeedAbs(); }
double    Car::getMaxSpeed()     const { return physics.getMaxSpeed(); }
glm::mat4 Car::getWorldMatrix()  const { return renderer.getWorldMatrix(); }