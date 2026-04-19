#pragma once


// Updated by CarInputController, used by CarPhysics
struct CarInputState
{
    bool isAccelerating = false;
    bool isBraking = false;
    bool isHandbrake = false;
    double steeringAngle = 0.0; // Positive angle => going left

};

// Read-only, used by CarRenderer
struct CarPhysicsState
{
    double posX = 0.0;
    double posY = 0.0;
    double posZ = 0.0;
    double carHeading = 90.0; ///< Degrees, world-space
    double currSpeed = 0.0;
    double velX = 0.0;
    double velZ = 0.0;
};


