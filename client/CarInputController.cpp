#include "CarInputController.h"
#include <cmath>

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
        if (inputState.steeringAngle < -MAX_STEERING_ANGLE)
            inputState.steeringAngle = -MAX_STEERING_ANGLE;
    }
    else if (isSteeringLeft) {
        inputState.steeringAngle += STEERING_SPEED * deltaTime;
        if (inputState.steeringAngle > MAX_STEERING_ANGLE)
            inputState.steeringAngle = MAX_STEERING_ANGLE;
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
	if (angle > MAX_STEERING_ANGLE) angle = MAX_STEERING_ANGLE;
	if (angle < -MAX_STEERING_ANGLE) angle = -MAX_STEERING_ANGLE;
	mouseSteeringTarget = angle;
}

void CarInputController::setMouseSteering(bool active) {
	isMouseSteering = active;
	if (!active) mouseSteeringTarget = 0.0;
}

void CarInputController::setSteeringAngleDirect(double angle)
{
	if (angle > MAX_STEERING_ANGLE)  angle = MAX_STEERING_ANGLE;
	if (angle < -MAX_STEERING_ANGLE) angle = -MAX_STEERING_ANGLE;
	inputState.steeringAngle = angle;
}