#include "CarInputController.h"
#include <cmath>
#include "WorldConfig.h"

void CarInputController::updateSteeringAngle(double const deltaTime) {
    if (isMouseSteering) {
        double diff = mouseSteeringTarget - inputState.steeringAngle;
        double step = STEERING_SPEED * deltaTime;
        if (std::abs(diff) <= step)
            inputState.steeringAngle = mouseSteeringTarget;
        else
            inputState.steeringAngle += (diff > 0 ? step : -step);
        return; 
    }

    if (isSteeringRight) {
        inputState.steeringAngle -= STEERING_SPEED * deltaTime;
        if (inputState.steeringAngle < -MAX_STEERING_ANGLE_DEG)
            inputState.steeringAngle = -MAX_STEERING_ANGLE_DEG;
    }
    else if (isSteeringLeft) {
        inputState.steeringAngle += STEERING_SPEED * deltaTime;
        if (inputState.steeringAngle > MAX_STEERING_ANGLE_DEG)
            inputState.steeringAngle = MAX_STEERING_ANGLE_DEG;
    }
    else if (inputState.steeringAngle > 0) {
        inputState.steeringAngle -= STEERING_SPEED * deltaTime;
        if (inputState.steeringAngle < 0.0) inputState.steeringAngle = 0.0;
    }
    else if (inputState.steeringAngle < 0) {
        inputState.steeringAngle += STEERING_SPEED * deltaTime;
        if (inputState.steeringAngle > 0.0) inputState.steeringAngle = 0.0;
    }
}

void CarInputController::setMouseSteeringTarget(double angle) {
	if (angle > MAX_STEERING_ANGLE_DEG) angle = MAX_STEERING_ANGLE_DEG;
	if (angle < -MAX_STEERING_ANGLE_DEG) angle = -MAX_STEERING_ANGLE_DEG;
	mouseSteeringTarget = angle;
}

void CarInputController::setMouseSteering(bool active) {
	isMouseSteering = active;
	if (!active) mouseSteeringTarget = 0.0;
}

void CarInputController::setSteeringAngleDirect(double angle)
{
	if (angle > MAX_STEERING_ANGLE_DEG)  angle = MAX_STEERING_ANGLE_DEG;
	if (angle < -MAX_STEERING_ANGLE_DEG) angle = -MAX_STEERING_ANGLE_DEG;
	inputState.steeringAngle = angle;
}