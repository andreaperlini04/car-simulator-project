#define _USE_MATH_DEFINES
#include "CarRenderer.h"
#include "engine.h"
#include <iostream>
#include <cmath>

bool CarRenderer::init(Node* passedNode, double& outPosX, double& outPosY, double& outPosZ) {

    Node* rootScene = passedNode;
    while (rootScene->getParent() != nullptr)
        rootScene = rootScene->getParent();

    this->carModel = rootScene->findByName("Car");
    if (!this->carModel) return false;

    glm::mat4 ovoMatrix = this->carModel->getM();

    outPosX = ovoMatrix[3][0];
    outPosY = ovoMatrix[3][1];
    outPosZ = ovoMatrix[3][2];

    // IMPOSTIAMO LA MATRICE INIZIALE DELLA MACCHINA 
    glm::mat4 startMatrix = glm::mat4(1.0f);
    startMatrix = glm::translate(startMatrix, glm::vec3((float)outPosX, (float)outPosY, (float)outPosZ));
    this->carModel->setM(startMatrix);

    //  RECUPERIAMO E AGGANCIAMO LE RUOTE
    for (int i = 0; i < 4; i++) {
        Node* ruota = rootScene->findByName(WHEEL_NAMES[i]);
        if (!ruota) continue;

        // Salviamo la matrice world originaria estratta dall'OVO
        glm::mat4 origRuotaWorld = ruota->getWorldFinalMatrix();

        // Calcoliamo la matrice relativa rispetto alla macchina
        glm::mat4 relRuotaM = glm::inverse(startMatrix) * origRuotaWorld; // 

        // Facciamo il reparenting
        if (ruota->getParent()) {
           // Rimuoviamo il figlio riottenendo il possesso esclusivo
           Node* extractedWheel = ruota->getParent()->removeChild(ruota);
           // Lo affidiamo al nuovo padre
           this->carModel->addChild(extractedWheel);
        }
        ruota->setM(relRuotaM);

        wheels[i].init(ruota, WHEEL_RADIUS, WHEEL_OFFSET_X, WHEEL_OFFSET_Y, WHEEL_OFFSET_Z);
    }

    wheels[2].setSteeringAngle(0.0);
    wheels[3].setSteeringAngle(0.0);

    //DEBUG
    for (int i = 0; i < 4; i++) {
        std::cout << wheels[i].getNode()->getName() << std::endl;
    }
    return true;
}



void CarRenderer::update(const CarPhysicsState& physics, double steeringAngle, double deltaTime) {
    if (this->carModel != nullptr) {
        glm::mat4 newMatrix = glm::mat4(1.0f);
        
        double carHeadingRad = physics.carHeading * (M_PI / 180.0);
        
        
        newMatrix = glm::translate(newMatrix, glm::vec3((float)physics.posX, (float)physics.posY, (float)physics.posZ));
        newMatrix = glm::rotate(newMatrix, (float)carHeadingRad, glm::vec3(0.0f, 1.0f, 0.0f));



        carModel->setM(newMatrix);

        // --- INERTIA ---
        // Calcoliamo la vera velocit� tangenziale per far ruotare correttamente i modelli 3D
        double actualTangentialSpeed = std::sqrt(physics.velX * physics.velX + physics.velZ * physics.velZ);

        // Se stiamo andando in retromarcia (currSpeed < 0), invertiamo il verso di rotazione
        if (physics.currSpeed < 0) {
            actualTangentialSpeed = -actualTangentialSpeed;
        }

        double distanceMoved = actualTangentialSpeed * deltaTime;

        wheels[0].setSteeringAngle(steeringAngle);
        wheels[1].setSteeringAngle(steeringAngle);


        for (int i = 0; i < 4; i++) {
            wheels[i].updateRolling(distanceMoved);
            wheels[i].updateVisuals();
        }
    }
}

glm::mat4 CarRenderer::getWorldMatrix() const {
	if (carModel) 
		return carModel->getWorldFinalMatrix();
	return glm::mat4(1.0f);
}

