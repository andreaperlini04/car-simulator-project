#include "Car.h"
#include "engine.h"
#include <iostream>

Car::Car()
{
    // TEMP VALUES: 
    //TODO: modificarle / spostarle
     
    this->isEngineOn = false;
    this->isAccelerating = false;
    this->isBraking = false;

    this->maxSpeed = 50.0;
    this->currSpeed = 0.0;
    this->accellerationFactor = 15.0;
    this->brakingFactor = 30.0;
    this->friction = 5.0; 

    this->carHeading = 90.0;

    this->steeringAngle = 0.0;
}

bool Car::startEngine()
{
    return this->isEngineOn = true;
    // animations should start
}

bool Car::turnOffEngine()
{
    return this->isEngineOn = false;
}

void Car::setAccelerating(bool isAccelerating)
{
    this->isAccelerating = isAccelerating;
}

void Car::setBraking(bool isBraking)
{
    this->isBraking = isBraking;
}

void Car::setSteering(double angle)
{
    this->steeringAngle = angle;
}

void Car::update(double deltaTime)
{
    if (deltaTime < 0) return;


    if (isAccelerating) {
        currSpeed += accellerationFactor * deltaTime;
    }
    else if (isBraking) {
        currSpeed -= brakingFactor * deltaTime;
    }
    else { // Terrain friction
        
        if (currSpeed > 0) {
            currSpeed -= friction * deltaTime;
            
            if (currSpeed < 0) // Avoids reversing 
                currSpeed = 0;
        }
        else if (currSpeed < 0) {
            currSpeed += friction * deltaTime;
            
            if (currSpeed > 0)
                currSpeed = 0;
        }
    }

    // Limits
    if (currSpeed > maxSpeed) currSpeed = maxSpeed;
    if (currSpeed < -maxSpeed) currSpeed = -maxSpeed;

    ///////
    /// STEERING
    ///////
    if (currSpeed != 0) {
        //steering* currSpeed* friction* deltaTime;
        double turnRate = steeringAngle * currSpeed * 0.1;  //0.1: factor that avoids spinning

        double grip = friction / (std::abs(currSpeed) * 0.25 + 1.0);
        if (grip > 1.0) grip = 1.0;

        // turnRate* grip = angolar speed
        carHeading += turnRate * grip * deltaTime;    
    }


    // Update position of the Car

    double carHeadingRad = carHeading * (3.14159 / 180.0); // Trasformo in rad
    posX += currSpeed * std::sin(carHeadingRad) * deltaTime;
    posZ += currSpeed * std::cos(carHeadingRad) * deltaTime;

    ///////
    /// VISUAL UPDATE
    ///////
    if (carModel != nullptr) {
        glm::mat4 newMatrix = glm::mat4(1.0f);
        newMatrix = glm::translate(newMatrix, glm::vec3(posX, 0.0f, posZ));
        newMatrix = glm::rotate(newMatrix, (float)carHeadingRad, glm::vec3(0.0f, 1.0f, 0.0f)); // steering
        carModel->setM(newMatrix);
    }

}


void Car::init(Node* passedNode, int startX, int startZ)
{
    this->posX = startX;
    this->posZ = startZ;

   // Dal log vediamo che Car e Ruote sono fratelli. Dobbiamo cercare partendo dal padre comune.
   Node* rootScene = passedNode;

   // Risaliamo la gerarchia finché non troviamo il nodo "Root" (quello senza padri)
   while (rootScene->getParent() != nullptr) {
      rootScene = rootScene->getParent();
   }

   std::cout << "Init Car: Ricerca iniziata dal nodo radice: '" << rootScene->getName() << "'" << std::endl;

   // --- 2. CERCA LA CARROZZERIA ---
   this->carModel = rootScene->findByName("Car");

   if (this->carModel == nullptr) {
      std::cout << "ERRORE CRITICO: Nodo 'Car' non trovato nella scena!" << std::endl;
      return;
   }
   std::cout << " -> Trovato corpo macchina: " << this->carModel->getName() << std::endl;

   std::string wheelNames[4] = { "RuotaAD", "RuotaAS", "RuotaPD", "RuotaPS" };
   std::string rimNames[4] = { "CerchioneAD", "CerchioneAS", "CerchionePD", "CerchionePS" };

   // --- 4. COLLEGAMENTO RUOTE E CERCHIONI ---
   for (int i = 0; i < 4; i++) {
      // --- A. GESTIONE GOMMA ---
      Node* ruota = rootScene->findByName(wheelNames[i]);
      if (ruota) {
         // 1. Calcola posizione relativa (per non farle saltare via)
         glm::mat4 relativeM = glm::inverse(this->carModel->getWorldFinalMatrix()) * ruota->getWorldFinalMatrix();

         // 2. Stacca dal vecchio padre (ID:12 [root])
         if (ruota->getParent()) {
            ruota->getParent()->removeChild(ruota);
         }

         // 3. Attacca alla macchina (Car diventa il nuovo padre)
         this->carModel->addChild(ruota);

         // 4. Applica la matrice corretta
         ruota->setM(relativeM);

         // 5. Inizializza oggetto logico
         wheels[i].init(ruota, 1.0f, 0.0f, 0.0f, 0.0f);

         std::cout << "    -> Collegata OK: " << wheelNames[i] << std::endl;
      }
      else {
         std::cout << "    ERRORE: " << wheelNames[i] << " NON trovata (cercando da " << rootScene->getName() << ")" << std::endl;
      }

      // --- B. GESTIONE CERCHIONE ---
      Node* cerchione = rootScene->findByName(rimNames[i]);
      if (cerchione) {
         glm::mat4 relativeM = glm::inverse(this->carModel->getWorldFinalMatrix()) * cerchione->getWorldFinalMatrix();

         if (cerchione->getParent()) {
            cerchione->getParent()->removeChild(cerchione);
         }

         this->carModel->addChild(cerchione);
         cerchione->setM(relativeM);

         std::cout << "    -> Collegato OK: " << rimNames[i] << std::endl;
      }
   }
}

glm::mat4 Car::getWorldMatrix() const
{
   if (carModel)
      return carModel->getWorldFinalMatrix();
   return glm::mat4(1.0f); // Matrice identità se non c'è modello
}

void Car::testDrive(float speed, float rotation)
{
   if (!carModel) return;

   // Sposta la macchina lungo il suo asse Z locale (Avanti/Indietro)
   // Nota: In molti engine OpenGL -Z è "avanti"
   carModel->translate(glm::vec3(0.0f, 0.0f, speed));

   // Ruota la macchina attorno all'asse Y (Sterzata)
   if (rotation != 0.0f) {
      carModel->rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f));
   }
}
