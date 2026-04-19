#pragma once
#include "node.h"
#include "CarInputController.h"
#include "CarPhysics.h"
#include "CarRenderer.h"

/**
 * @brief Facade che compone CarInputController, CarPhysics e CarRenderer.
 *
 * Questa classe espone la stessa API pubblica della classe Car originale, ma
 * delega ogni responsabilita' al sottosistema appropriato, 
 *
 */
class Car
{
public:
    Car(double maxSpeed,
        double accelerationFactor,
        double brakingFactor,
        double friction,
        double reverseGearMaxSpeed);

    //  Ciclo di vita 
    void init(Node* carNode);

    bool startEngine();
    bool turnOffEngine();
    bool isEngineStarted() const;

    // Input (delegati a CarInputController) 
    void setAccelerating(bool v);
    void setBraking(bool v);
    void setHandbrake(bool v);
    void setSteeringLeft(bool v);
    void setSteeringRight(bool v);
    void setSteeringAngleDirect(double angle);
    void setMouseSteeringTarget(double angle);
    void setMouseSteering(bool active);

    void update(double deltaTime);

    // Lettura
    double getCurrSpeedAbs()  const;
    double getMaxSpeed()      const;
    glm::mat4 getWorldMatrix()   const;

private:
    CarInputController inputController;
    CarPhysics physics;
    CarRenderer renderer;
};