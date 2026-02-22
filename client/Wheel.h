#pragma once
#include "node.h"

class Wheel
{
public:
    Wheel();

    void init(Node* model, double wheelRadius, double offX, double offY, double offZ);

    void setSteeringAngle(double angle);

    void updateRolling(double distanceMoved);

    void updateVisuals();

private:
    Node* wheelModel;

    double radius; 

    // Related to the centre of the car
    double offsetX;
    double offsetY;
    double offsetZ;

    double steeringAngle;
    double rollingAngle; 
};