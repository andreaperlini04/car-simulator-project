#pragma once
#include "node.h"
#include "wheel.h"


class Car
{
public:	
	Car(Node* carModel, int startX, int startZ);
	
	bool startEngine();
	bool turnOffEngine();
	
	// Player input
	void setAccelerating(bool isAccelerating);
	void setBraking(bool isBraking);
	void setSteering(double angle);

	void update(double deltaTime); 

private:
	Node* carModel;
	Wheel wheels[4];

	// CONST ?? 

	double maxSpeed;
	double currSpeed;
	double accellerationFactor;
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

