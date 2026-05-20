/**
 * @file client_test.cpp
 * @brief Unit test per il modulo simulativo (Fisica, Input, Ruote).
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>

#include "CarInputController.h"
#include "CarPhysics.h"
#include "Wheel.h"
#include "WorldConfig.h"
#include "node.h"

const double EPSILON = 0.001;

bool isClose(double a, double b) {
   return std::abs(a - b) < EPSILON;
}

void test_CarInputController() {
   std::cout << "[TEST] CarInputController... ";

   CarInputController input;

   input.setSteeringLeft(true);
   input.updateSteeringAngle(0.5);
   assert(input.getState().steeringAngle > 0.0);

   input.updateSteeringAngle(10.0);
   assert(isClose(input.getState().steeringAngle, MAX_STEERING_ANGLE_DEG));

   input.setSteeringLeft(false);
   input.updateSteeringAngle(1.0);
   assert(input.getState().steeringAngle < MAX_STEERING_ANGLE_DEG);

   input.setMouseSteering(true);
   input.setMouseSteeringTarget(15.0);
   input.updateSteeringAngle(0.1);
   assert(input.getState().steeringAngle > 0.0);

   std::cout << "OK\n";
}

void test_CarPhysics() {
   std::cout << "[TEST] CarPhysics base... ";

   CarPhysics physics(50.0, 10.0, 20.0, 1.0, 15.0);
   CarInputState input;

   input.isAccelerating = true;
   physics.update(1.0, input);
   assert(isClose(physics.getState().currSpeed, 0.0));

   physics.startEngine();
   physics.update(1.0, input);
   assert(physics.getState().currSpeed > 0.0);

   physics.update(10.0, input);
   assert(isClose(physics.getState().currSpeed, 50.0));

   input.isAccelerating = false;
   input.isBraking = true;
   physics.update(20.0, input);
   assert(isClose(physics.getState().currSpeed, -15.0));

   input.isBraking = false;
   physics.update(1.0, input);
   assert(physics.getState().currSpeed > -15.0 && physics.getState().currSpeed < 0.0);

   // Verifica prevenzione esplosione numerica dell'integratore in caso di lag spike
   double speedBeforeLag = physics.getState().currSpeed;
   physics.update(5.0, input);
   assert(isClose(physics.getState().currSpeed, speedBeforeLag));
   physics.update(-0.5, input);
   assert(isClose(physics.getState().currSpeed, speedBeforeLag));

   std::cout << "OK\n";

   std::cout << "[TEST] CarPhysics Tire Scrub e Drift... ";

   // Attrito azzerato per isolare matematicamente la dissipazione cinetica indotta dallo scrub
   CarPhysics physScrub(50.0, 10.0, 20.0, 0.0, 15.0);
   physScrub.startEngine();
   CarInputState inScrub;

   inScrub.isAccelerating = true;
   physScrub.update(2.0, inScrub);
   inScrub.isAccelerating = false;

   double speedBeforeScrub = physScrub.getState().currSpeed;

   inScrub.steeringAngle = 35.0;
   physScrub.update(1.0, inScrub);

   assert(physScrub.getState().currSpeed < speedBeforeScrub);

   CarPhysics physDrift(50.0, 10.0, 20.0, 1.0, 15.0);
   physDrift.startEngine();
   CarInputState inDrift;

   inDrift.isAccelerating = true;
   physDrift.update(2.0, inDrift);

   double speedBeforeDrift = physDrift.getState().currSpeed;
   double headingBeforeDrift = physDrift.getState().carHeading;

   inDrift.isAccelerating = false;
   inDrift.isHandbrake = true;
   inDrift.steeringAngle = 35.0;

   physDrift.update(0.5, inDrift);

   // Il driftTurnBoost deve forzare una rotazione asimmetrica dell'heading
   assert(physDrift.getState().carHeading != headingBeforeDrift);

   // Lo slittamento laterale aumenta la resistenza al suolo, frenando il veicolo
   assert(physDrift.getState().currSpeed < speedBeforeDrift);

   std::cout << "OK\n";
}

void test_Wheel() {
   std::cout << "[TEST] Wheel Solver... ";

   Wheel wheel;
   auto dummyWheelNode = std::make_unique<Node>("RuotaTest");
   dummyWheelNode->translate(glm::vec3(0.0f, 2.0f, 0.0f));

   wheel.init(dummyWheelNode.get());

   double distanceMoved = 2.0 * glm::pi<double>();
   wheel.updateRolling(distanceMoved);

   glm::mat4 beforeMatrix = dummyWheelNode->getM();
   wheel.updateVisuals();
   glm::mat4 afterMatrix = dummyWheelNode->getM();

   assert(beforeMatrix != afterMatrix);

   std::cout << "OK\n";
}

// Test: Engine on/off state transitions
void test_CarPhysics_EngineState() {
   std::cout << "[TEST] CarPhysics Engine on/off... ";

   CarPhysics physics(50.0, 10.0, 20.0, 1.0, 15.0);

   // Engine deve partire spento
   assert(!physics.isEngineStarted());

   // startEngine() deve riuscire la prima volta
   assert(physics.startEngine());
   assert(physics.isEngineStarted());

   // startEngine() su motore gia' acceso deve fallire
   assert(!physics.startEngine());

   // turnOffEngine() deve spegnerlo
   assert(physics.turnOffEngine());
   assert(!physics.isEngineStarted());

   // turnOffEngine() su motore gia' spento deve fallire
   assert(!physics.turnOffEngine());

   // Con motore spento, accelerare non deve cambiare la velocita'
   CarInputState inputOff;
   inputOff.isAccelerating = true;
   physics.update(1.0, inputOff);
   assert(isClose(physics.getState().currSpeed, 0.0));

   std::cout << "OK\n";
}

// Test: Position integration
void test_CarPhysics_PositionIntegration() {
   std::cout << "[TEST] CarPhysics position integration... ";

   CarPhysics physics(50.0, 10.0, 20.0, 0.0, 15.0); // friction=0 per isolamento
   physics.startEngine();

   CarInputState input;
   input.isAccelerating = true;

   // Dopo diversi step il veicolo deve essersi spostato
   physics.update(1.0, input);
   physics.update(1.0, input);

   double dx = physics.getState().posX;
   double dz = physics.getState().posZ;
   assert(dx * dx + dz * dz > 0.0);

   std::cout << "OK\n";
}

// Test: Max forward speed clamping
void test_CarPhysics_MaxSpeedClamp() {
   std::cout << "[TEST] CarPhysics max speed clamping... ";

   const double maxSpeed = 50.0;
   CarPhysics physics(maxSpeed, 10.0, 20.0, 0.0, 15.0);
   physics.startEngine();

   CarInputState input;
   input.isAccelerating = true;

   // Applica moltissimi tick di accelerazione
   for (int i = 0; i < 200; ++i)
      physics.update(0.5, input);

   // La velocita' non deve mai superare maxSpeed
   assert(physics.getState().currSpeed <= maxSpeed + EPSILON);

   std::cout << "OK\n";
}

// Test: Reverse gear speed cap
void test_CarPhysics_ReverseSpeedCap() {
   std::cout << "[TEST] CarPhysics reverse speed cap... ";

   const double reverseMax = 15.0;
   CarPhysics physics(50.0, 10.0, 20.0, 0.0, reverseMax);
   physics.startEngine();

   CarInputState input;
   input.isBraking = true; // Freno da fermo = retromarcia

   for (int i = 0; i < 200; ++i)
      physics.update(0.5, input);

   // La velocita' in retromarcia non deve superare reverseMax (in valore assoluto)
   assert(physics.getState().currSpeed >= -(reverseMax + EPSILON));

   std::cout << "OK\n";
}

// Test: Heading changes when steering
void test_CarPhysics_HeadingChange() {
   std::cout << "[TEST] CarPhysics heading change on steer... ";

   CarPhysics physics(50.0, 10.0, 20.0, 0.0, 15.0);
   physics.startEngine();

   CarInputState input;
   input.isAccelerating = true;

   // Porta il veicolo a velocita'
   physics.update(2.0, input);

   double headingBefore = physics.getState().carHeading;

   // Applicazione sterzo
   input.steeringAngle = 25.0;
   physics.update(1.0, input);
   assert(!isClose(physics.getState().carHeading, headingBefore));

   // Senza sterzo l'heading non deve cambiare ulteriormente
   input.steeringAngle = 0.0;
   double headingNeutral = physics.getState().carHeading;
   physics.update(1.0, input);
   assert(isClose(physics.getState().carHeading, headingNeutral));

   std::cout << "OK\n";
}

// Test: setInitialPosition resets state
void test_CarPhysics_InitialPosition() {
   std::cout << "[TEST] CarPhysics setInitialPosition reset... ";

   CarPhysics physics(50.0, 10.0, 20.0, 0.0, 15.0);
   physics.startEngine();

   CarInputState input;
   input.isAccelerating = true;
   physics.update(2.0, input);

   assert(physics.getState().currSpeed > 0.0);

   const double spawnX = 100.0, spawnY = 0.0, spawnZ = 200.0;
   physics.setInitialPosition(spawnX, spawnY, spawnZ);

   assert(isClose(physics.getState().posX, spawnX));
   assert(isClose(physics.getState().posY, spawnY));
   assert(isClose(physics.getState().posZ, spawnZ));

   // L'inerzia deve essere azzerata dopo il reset
   assert(isClose(physics.getState().currSpeed, 0.0));
   assert(isClose(physics.getState().velX, 0.0));
   assert(isClose(physics.getState().velZ, 0.0));

   std::cout << "OK\n";
}

// Test: CarInputController – right steering, neutral return, mouse target zero
void test_CarInputController_Extended() {
   std::cout << "[TEST] CarInputController neutral return & right steer... ";

   CarInputController input;

   // Sterzo a sinistra al massimo
   input.setSteeringLeft(true);
   input.updateSteeringAngle(100.0);
   assert(isClose(input.getState().steeringAngle, MAX_STEERING_ANGLE_DEG));

   // Senza input, lo sterzo deve tornare verso zero
   input.setSteeringLeft(false);
   input.updateSteeringAngle(0.5);
   assert(input.getState().steeringAngle < MAX_STEERING_ANGLE_DEG);

   // Sterzo a destra al massimo
   input.setSteeringRight(true);
   input.updateSteeringAngle(100.0);
   assert(isClose(input.getState().steeringAngle, -MAX_STEERING_ANGLE_DEG));

   // Senza input, deve tornare verso zero
   input.setSteeringRight(false);
   input.updateSteeringAngle(0.5);
   assert(input.getState().steeringAngle > -MAX_STEERING_ANGLE_DEG);

   // Mouse target a zero: l'angolo deve convergere verso zero
   input.setMouseSteering(true);
   input.setMouseSteeringTarget(0.0);
   input.updateSteeringAngle(1.0);
   assert(std::abs(input.getState().steeringAngle) < MAX_STEERING_ANGLE_DEG);

   std::cout << "OK\n";
}

int main() {
   std::cout << "--- AVVIO TEST SUITE FISICA ---\n";

   test_CarInputController();
   test_CarInputController_Extended();
   test_CarPhysics();
   test_CarPhysics_EngineState();
   test_CarPhysics_PositionIntegration();
   test_CarPhysics_MaxSpeedClamp();
   test_CarPhysics_ReverseSpeedCap();
   test_CarPhysics_HeadingChange();
   test_CarPhysics_InitialPosition();
   test_Wheel();

   std::cout << "--- TUTTI I TEST SUPERATI ---\n";

   return 0;
}
