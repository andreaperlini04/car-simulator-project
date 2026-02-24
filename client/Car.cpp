#include "Car.h"
#include <iostream>

Car::Car()
{
}

bool Car::startEngine()
{
    return true; 
}

bool Car::turnOffEngine()
{
    return true; 
}

void Car::setAccelerating(bool isAccelerating)
{
}

void Car::setBraking(bool isBraking)
{
}

void Car::setSteering(double angle)
{
}

void Car::update()
{
}

void Car::init(Node* passedNode)
{
   // --- 1. RISALITA ALLA RADICE (FIX PER IL TUO ERRORE) ---
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

   // --- 3. LISTA NOMI (Esatti dal tuo log) ---
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

         // Opzionale: Se vuoi che il cerchione giri con la ruota, dovresti attaccarlo a 'ruota' invece che a 'carModel'.
         // Per ora lo lasciamo attaccato alla macchina come da tua richiesta precedente.
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
