/**
 * @file client_test.cpp
 * @brief Test suite per la logica del Client (Fisica, Input, Ruote)
 */

#include <iostream>
#include <cassert>
#include <cmath>

 // Include i file del client
#include "CarInputController.h"
#include "CarPhysics.h"
#include "Wheel.h"
#include "WorldConfig.h"
#include "node.h"

// Costante di tolleranza per il confronto tra floating point
const double EPSILON = 0.001;

bool isClose(double a, double b) {
   return std::abs(a - b) < EPSILON;
}

// ========================================================================
// 1. TEST CAR INPUT CONTROLLER
// ========================================================================
void test_CarInputController() {
   std::cout << "[TEST] CarInputController (Sterzo e Clamp)... ";

   CarInputController input;

   // Test 1: Sterzo a sinistra (incrementa l'angolo)
   input.setSteeringLeft(true);
   input.updateSteeringAngle(0.5); // Simula 0.5 secondi
   assert(input.getState().steeringAngle > 0.0);

   // Test 2: Clamp dell'angolo massimo
   input.updateSteeringAngle(10.0);
   assert(isClose(input.getState().steeringAngle, MAX_STEERING_ANGLE_DEG));

   // Test 3: Ritorno al centro
   input.setSteeringLeft(false);
   input.updateSteeringAngle(1.0);
   assert(input.getState().steeringAngle < MAX_STEERING_ANGLE_DEG);

   // Test 4: Mouse Steering
   input.setMouseSteering(true);
   input.setMouseSteeringTarget(15.0);
   input.updateSteeringAngle(0.1);
   assert(input.getState().steeringAngle > 0.0);

   std::cout << "OK" << std::endl;
}

// ========================================================================
// 2. TEST CAR PHYSICS
// ========================================================================
void test_CarPhysics() {
   std::cout << "[TEST] CarPhysics (Accelerazione, Limiti, Inerzia)... ";

   CarPhysics physics(50.0, 10.0, 20.0, 1.0, 15.0);
   CarInputState input;

   // Test 1: Motore spento -> Nessuna accelerazione
   input.isAccelerating = true;
   physics.update(1.0, input);
   assert(isClose(physics.getState().currSpeed, 0.0));

   // Test 2: Accensione e accelerazione base
   physics.startEngine();
   physics.update(1.0, input); // 1 sec a 10m/s^2 di accelerazione
   assert(physics.getState().currSpeed > 0.0);

   // Test 3: Tetto massimo della velocità in avanti
   physics.update(10.0, input);
   assert(isClose(physics.getState().currSpeed, 50.0));

   // Test 4: Frenata e retromarcia (tetto massimo retromarcia)
   input.isAccelerating = false;
   input.isBraking = true;
   physics.update(20.0, input);
   assert(isClose(physics.getState().currSpeed, -15.0));

   // Test 5: Attrito (Friction) a input neutro
   input.isBraking = false;
   physics.update(1.0, input);
   assert(physics.getState().currSpeed > -15.0 && physics.getState().currSpeed < 0.0);

   // Test 6: Protezione DeltaTime (Lag spike guard in CarPhysics::update)
   double speedBeforeLag = physics.getState().currSpeed;
   physics.update(5.0, input); // 5.0s è > MAX_DELTA_TIME (1.0). Deve fare return.
   assert(isClose(physics.getState().currSpeed, speedBeforeLag));
   physics.update(-0.5, input); // Tempo negativo non valido. Deve fare return.
   assert(isClose(physics.getState().currSpeed, speedBeforeLag));

   std::cout << "OK" << std::endl;

   std::cout << "[TEST] CarPhysics (Tire Scrub e Drift)... ";

   // --- Test 7: Tire Scrub (Rallentamento dovuto a sterzata brusca) ---
   // Attrito a 0 per isolare matematicamente l'effetto dello scrub
   CarPhysics physScrub(50.0, 10.0, 20.0, 0.0, 15.0);
   physScrub.startEngine();
   CarInputState inScrub;

   // Acceleriamo per 2 secondi a 10m/s^2 -> velocità ~20
   inScrub.isAccelerating = true;
   physScrub.update(2.0, inScrub);
   inScrub.isAccelerating = false;

   double speedBeforeScrub = physScrub.getState().currSpeed;

   // Sterziamo al massimo per 1 secondo senza gas né freni
   inScrub.steeringAngle = 35.0;
   physScrub.update(1.0, inScrub);

   // La velocità DEVE essere diminuita a causa di applyTireScrub()
   assert(physScrub.getState().currSpeed < speedBeforeScrub);

   // --- Test 8: Drift e Perdita di Aderenza ---
   CarPhysics physDrift(50.0, 10.0, 20.0, 1.0, 15.0);
   physDrift.startEngine();
   CarInputState inDrift;

   // Acceleriamo sopra al DRIFT_MIN_SPEED (che nel tuo file è 5.0)
   inDrift.isAccelerating = true;
   physDrift.update(2.0, inDrift); // Velocità ~20.0

   double speedBeforeDrift = physDrift.getState().currSpeed;
   double headingBeforeDrift = physDrift.getState().carHeading;

   // Inneschiamo il drift tirando il freno a mano e sterzando
   inDrift.isAccelerating = false;
   inDrift.isHandbrake = true;
   inDrift.steeringAngle = 35.0; // Sterzo al massimo

   physDrift.update(0.5, inDrift);

   // 1. L'Heading deve essere cambiato pesantemente (driftTurnBoost = 2.5)
   assert(physDrift.getState().carHeading != headingBeforeDrift);

   // 2. La macchina deve aver perso velocità velocemente per via del "slideDecel"
   assert(physDrift.getState().currSpeed < speedBeforeDrift);

   std::cout << "OK" << std::endl;
}

// ========================================================================
// 3. TEST WHEEL (Rotolamento)
// ========================================================================
void test_Wheel() {
   std::cout << "[TEST] Wheel (Calcolo rotolamento)... ";

   Wheel wheel;
   Node* dummyWheelNode = new Node("RuotaTest");
   dummyWheelNode->translate(glm::vec3(0.0f, 2.0f, 0.0f));

   wheel.init(dummyWheelNode, 1.0f, 0.0, 0.0, 0.0);

   double distanceMoved = 2.0 * glm::pi<double>();
   wheel.updateRolling(distanceMoved);

   glm::mat4 beforeMatrix = dummyWheelNode->getM();
   wheel.updateVisuals();
   glm::mat4 afterMatrix = dummyWheelNode->getM();

   assert(beforeMatrix != afterMatrix);

   delete dummyWheelNode;
   std::cout << "OK" << std::endl;
}

// ========================================================================
// MAIN TEST RUNNER
// ========================================================================
#ifndef ALL_TESTS_UNIFIED
int main() {
   std::cout << "==========================================" << std::endl;
   std::cout << "      AVVIO CLIENT TEST SUITE (MAIN)      " << std::endl;
   std::cout << "==========================================" << std::endl;

   test_CarInputController();
   test_CarPhysics();
   test_Wheel();

   std::cout << "==========================================" << std::endl;
   std::cout << "  TUTTI I TEST CLIENT SUPERATI CON SUCCESSO" << std::endl;
   std::cout << "==========================================" << std::endl;

   return 0;
}
#endif