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

// ---------------------------------------------------------------------------
// FIXED TIMESTEP — passo fisico
// ---------------------------------------------------------------------------
// Chiamato N volte per frame dentro il loop accumulatore di displayCallback.
// Riceve SEMPRE lo stesso dt fisso: la fisica e' deterministica e
// indipendente dal frame-rate del rendering.
// ---------------------------------------------------------------------------
void Car::stepPhysics(double fixedDt)
{
   // 1. Aggiorna l'angolo di sterzata con il timestep fisso.
   //    In questo modo la velocita' di sterzata e' identica a 30 FPS e a 144 FPS.
   inputController.updateSteeringAngle(fixedDt);

   // 2. Integra la fisica (velocita', posizione, inerzia, drift).
   physics.update(fixedDt, inputController.getState());
}

// ---------------------------------------------------------------------------
// FIXED TIMESTEP — aggiornamento renderer
// ---------------------------------------------------------------------------
// Chiamato UNA SOLA VOLTA per frame, dopo aver eseguito tutti i passi fisici.
// Il renderer legge lo stato fisico gia' aggiornato e aggiorna la matrice
// mondo del modello 3D + animazione ruote.
//
// frameDt = tempo reale del frame (variabile): usato SOLO per l'animazione
// visiva delle ruote (distanceMoved). NON influenza la simulazione fisica.
//
// NOTA: per un rendering ancora piu' preciso si potrebbe implementare
// l'interpolazione lineare tra lo stato fisico del frame precedente e quello
// corrente usando alpha = accumulator / FIXED_DT. Per ora non e' necessario.
// ---------------------------------------------------------------------------
void Car::updateRenderer(double frameDt)
{
   renderer.update(physics.getState(), inputController.getState().steeringAngle, frameDt);
}

// Lettura
double    Car::getCurrSpeedAbs() const { return physics.getCurrSpeedAbs(); }
double    Car::getMaxSpeed()     const { return physics.getMaxSpeed(); }
glm::mat4 Car::getWorldMatrix()  const { return renderer.getWorldMatrix(); }