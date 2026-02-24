#include "Car.h"
#include "engine.h"


Car::Car(Node* carModel, int startX, int startZ)
{
    this->carModel = carModel;

    // TEMP VALUES: 
    //TODO: modificarle / spostarle
     
    this->isEngineOn = false;
    this->isAccelerating = false;
    this->isBraking = false;

    this->maxSpeed = 50.0;
    this->currSpeed = 0.0;
    this->accellerationFactor = 15.0;
    this->brakingFactor = 30.0;
    this->friction = 5.0; 

    this->posX = startX;
    this->posZ = startZ;

    this->carHeading = 90.0;

    this->steeringAngle = 0.0;
}

bool Car::startEngine()
{
    return this->isEngineOn = true;
    // animations should start
}

bool Car::turnOffEngine()
{
    return this->isEngineOn = false;
}

void Car::setAccelerating(bool isAccelerating)
{
    this->isAccelerating = isAccelerating;
}

void Car::setBraking(bool isBraking)
{
    this->isBraking = isBraking;
}

void Car::setSteering(double angle)
{
    this->steeringAngle = angle;
}

void Car::update(double deltaTime)
{
    if (deltaTime < 0) return;


    if (isAccelerating) {
        currSpeed += accellerationFactor * deltaTime;
    }
    else if (isBraking) {
        currSpeed -= brakingFactor * deltaTime;
    }
    else { // Terrain friction
        
        if (currSpeed > 0) {
            currSpeed -= friction * deltaTime;
            
            if (currSpeed < 0) // Avoids reversing 
                currSpeed = 0;
        }
        else if (currSpeed < 0) {
            currSpeed += friction * deltaTime;
            
            if (currSpeed > 0)
                currSpeed = 0;
        }
    }

    // Limits
    if (currSpeed > maxSpeed) currSpeed = maxSpeed;
    if (currSpeed < -maxSpeed) currSpeed = -maxSpeed;

    ///////
    /// STEERING
    ///////
    if (currSpeed != 0) {
        //steering* currSpeed* friction* deltaTime;
        double turnRate = steeringAngle * currSpeed * 0.1;  //0.1: factor that avoids spinning

        double grip = friction / (std::abs(currSpeed) * 0.25 + 1.0);
        if (grip > 1.0) grip = 1.0;

        // turnRate* grip = angolar speed
        carHeading += turnRate * grip * deltaTime;    
    }


    // Update position of the Car

    double carHeadingRad = carHeading * (3.14159 / 180.0); // Trasformo in rad
    posX += currSpeed * std::sin(carHeadingRad) * deltaTime;
    posZ += currSpeed * std::cos(carHeadingRad) * deltaTime;

    ///////
    /// VISUAL UPDATE
    ///////
    if (carModel != nullptr) {
        glm::mat4 newMatrix = glm::mat4(1.0f);
        newMatrix = glm::translate(newMatrix, glm::vec3(posX, 0.0f, posZ));
        newMatrix = glm::rotate(newMatrix, (float)carHeadingRad, glm::vec3(0.0f, 1.0f, 0.0f)); // steering
        carModel->setM(newMatrix);
    }
}