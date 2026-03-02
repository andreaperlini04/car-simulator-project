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

   this->maxSpeed = 25.0;
   this->currSpeed = 0.0;
   this->accelerationFactor =8.0;
   this->brakingFactor = 20.0;
   this->friction = 10.0;

   this->carHeading = 90.0;
   this->steeringAngle = 0.0;

   this->posX = 0.0;
   this->posZ = 0.0;
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
      currSpeed += accelerationFactor * deltaTime;
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
      double turnRate = steeringAngle * currSpeed * 0.05;  //Literal: factor that avoids spinning

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
      newMatrix = glm::translate(newMatrix, glm::vec3(posX, originalY, posZ));

      // Aggiunge rotazione base di 180° per correggere l'orientamento del modello
      newMatrix = glm::rotate(newMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

      // Poi applica la rotazione di guida normale
      newMatrix = glm::rotate(newMatrix, (float)carHeadingRad, glm::vec3(0.0f, 1.0f, 0.0f));

      newMatrix = glm::scale(newMatrix, originalScale);

      carModel->setM(newMatrix);

      // Steering Animation
      if (currSpeed != 0) {     
          wheels[0].setSteeringAngle(steeringAngle);
          wheels[0].updateVisuals();
          wheels[1].setSteeringAngle(steeringAngle);
          wheels[1].updateVisuals();
      }

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

   this->carModel = rootScene->findByName("Car");

   if (this->carModel == nullptr) {
      std::cout << "ERRORE CRITICO: Nodo 'Car' non trovato nella scena!" << std::endl;
      return;
   }
   std::cout << " -> Trovato corpo macchina: " << this->carModel->getName() << std::endl;

   // FIX: Estrai e salva la scala originale dalla matrice del modello caricato dall'OVO.
   // Le colonne della matrice 3x3 superiore rappresentano gli assi scalati.
   // La lunghezza di ciascun vettore-colonna è il fattore di scala su quell'asse.
   // Usiamo la matrice LOCALE per estrarre la scala (le colonne 0-2 non sono influenzate dai padri)
   glm::mat4 localMatrix = this->carModel->getM();
   this->originalScale = glm::vec3(
      glm::length(glm::vec3(localMatrix[0])),  // scala X
      glm::length(glm::vec3(localMatrix[1])),  // scala Y
      glm::length(glm::vec3(localMatrix[2]))   // scala Z
   );
   // FIX: Per la Y usiamo la WORLD matrix: include il contributo di tutti i nodi padre
   // (la matrice locale da sola non basta se il parent ha una Y diversa da zero)
   glm::mat4 worldMatrix = this->carModel->getWorldFinalMatrix();
   this->originalY = worldMatrix[3][1];
   std::cout << " -> Scala originale estratta: ("
      << originalScale.x << ", " << originalScale.y << ", " << originalScale.z << ")"
      << "  Y originale (world): " << originalY << std::endl;

   glm::vec3 forwardVec = glm::normalize(glm::vec3(
      worldMatrix[2][0] / originalScale.x,
      worldMatrix[2][1] / originalScale.y,
      worldMatrix[2][2] / originalScale.z
   ));
   float initHeadingRad = atan2f(forwardVec.x, forwardVec.z);
   this->carHeading = (double)(initHeadingRad * 180.0 / 3.14159);

   // Anche posX/posZ li prendiamo dalla world matrix per essere coerenti
   this->posX = worldMatrix[3][0];
   this->posZ = worldMatrix[3][2];

   // Costruisci la cleanCarMatrix (ora replica fedelmente la matrice OVO)
   glm::mat4 cleanCarMatrix = glm::mat4(1.0f);
   cleanCarMatrix = glm::translate(cleanCarMatrix, glm::vec3((float)posX, originalY, (float)posZ));
   cleanCarMatrix = glm::rotate(cleanCarMatrix, initHeadingRad, glm::vec3(0.0f, 1.0f, 0.0f));
   cleanCarMatrix = glm::scale(cleanCarMatrix, originalScale);

   std::cout << " -> Heading estratto dall'OVO: " << this->carHeading << " deg" << std::endl;

  

   // --- 4. COLLEGAMENTO RUOTE E CERCHIONI ---
   for (int i = 0; i < 4; i++) {
      // --- A. GESTIONE GOMMA ---
      Node* ruota = rootScene->findByName(wheelNames[i]);
      if (ruota) {
         // 1. Calcola posizione relativa contro la worldMatrix OVO originale (PRIMA di setM)
         //    così la posizione delle ruote è coerente con la posizione reale del car nell'OVO.
         glm::mat4 relativeM = glm::inverse(worldMatrix) * ruota->getWorldFinalMatrix();

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
         glm::mat4 relativeM = glm::inverse(worldMatrix) * cerchione->getWorldFinalMatrix();

         if (cerchione->getParent()) {
            cerchione->getParent()->removeChild(cerchione);
         }

         this->carModel->addChild(cerchione);
         cerchione->setM(relativeM);

         std::cout << "    -> Collegato OK: " << rimNames[i] << std::endl;
      }
   }

   // Applica cleanCarMatrix DOPO aver calcolato tutte le relative,
   // così update() parte da una matrice coerente con i wheel locals.
   this->carModel->setM(cleanCarMatrix);
}

glm::mat4 Car::getWorldMatrix() const
{
   if (carModel)
      return carModel->getWorldFinalMatrix();
   return glm::mat4(1.0f); // Matrice identità se non c'è modello
}

