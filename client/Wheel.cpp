#include "wheel.h"

Wheel::Wheel()
{
    wheelModel = nullptr;
    radius = 1.0;

    offsetX = 0.0;
    offsetY = 0.0;
    offsetZ = 0.0;

    steeringAngle = 0.0;
    rollingAngle = 0.0;
}

void Wheel::init(Node* model, double wheelRadius, double offX, double offY, double offZ)
{
    wheelModel = model;
    radius = wheelRadius;
    offsetX = offX;
    offsetY = offY;
    offsetZ = offZ;
}

void Wheel::setSteeringAngle(double angle)
{
    steeringAngle = angle;
}

void Wheel::updateRolling(double distanceMoved)
{
}

void Wheel::updateVisuals()
{
    if (!wheelModel) return;


}