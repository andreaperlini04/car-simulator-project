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

   wheel.init(dummyWheelNode.get(), 1.0f, 0.0, 0.0, 0.0);

   double distanceMoved = 2.0 * glm::pi<double>();
   wheel.updateRolling(distanceMoved);

   glm::mat4 beforeMatrix = dummyWheelNode->getM();
   wheel.updateVisuals();
   glm::mat4 afterMatrix = dummyWheelNode->getM();

   assert(beforeMatrix != afterMatrix);

   std::cout << "OK\n";
}

#ifndef ALL_TESTS_UNIFIED
int main() {
   std::cout << "--- AVVIO TEST SUITE FISICA ---\n";

   test_CarInputController();
   test_CarPhysics();
   test_Wheel();

   std::cout << "--- TUTTI I TEST SUPERATI ---\n";

   return 0;
}
#endif