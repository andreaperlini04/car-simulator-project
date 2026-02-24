#pragma once
#include "node.h"
#include "wheel.h"


class Car
{
public:	
	Car();
	
	// Inizializza la macchina con il nodo grafico
	void init(Node* carNode);

	bool startEngine();
	bool turnOffEngine();
	
	// Player input
	void setAccelerating(bool isAccelerating);
	void setBraking(bool isBraking);
	void setSteering(double angle);

	void update(); //Called at every frame

	glm::mat4 getWorldMatrix() const;

	void testDrive(float speed, float rotation);

private:
	Node* carModel = nullptr;
	Wheel wheels[4];

	double maxSpeed;
	double currSpeed;
	double accellerationFactor;
	double brakingFactor;
	double friction;  // Terrain friction


	double posX;
	double posZ;

	double carHeading;    // Position at which the car is pointing (angle)
	double steeringAngle = 0; // Angle: is parallel to the front wheels



	bool isAccelerating;
	bool isBraking;
};

