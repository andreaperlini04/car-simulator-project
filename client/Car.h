#pragma once
#include "node.h"
#include "wheel.h"


class Car
{
public:	
	Car();
	
	// Inizializza la macchina con il nodo grafico
	void init(Node* carNode, int startX, int startZ);

	bool startEngine();
	bool turnOffEngine();
	
	// Player input
	void setAccelerating(bool isAccelerating);
	void setBraking(bool isBraking);
	void setSteering(double angle);


	void update(double deltaTime); 

	glm::mat4 getWorldMatrix() const;
	const std::string wheelNames[4] = { "RuotaAD", "RuotaAS", "RuotaPD", "RuotaPS" };
	const std::string rimNames[4] = { "CerchioneAD", "CerchioneAS", "CerchionePD", "CerchionePS" };

private:
	Node* carModel = nullptr;
	Wheel wheels[4];

	// CONST ?? 
	float originalY = 0.0f;
	glm::vec3 originalScale = glm::vec3(1.0f);



	double maxSpeed;
	double currSpeed;
	double accelerationFactor;
	double brakingFactor;
	double friction;  // Terrain friction


	double posX;
	double posZ;

	double carHeading;    // Position at which the car is pointing (angle)			 
	double steeringAngle = 90.0; // Angle: is parallel to the front wheels (Nord)    


	bool isEngineOn = false;
	bool isAccelerating;
	bool isBraking;
};

