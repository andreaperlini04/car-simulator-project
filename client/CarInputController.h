#pragma once
#include "CarState.h"

/*
 * @brief Gestisce tutto l'input grezzo del giocatore e produce
 * uno snapshot CarInputState da passare a CarPhysics.
 *
 * Compiti:
 *  - Gestione tasti e mouse.
 *  - Mantenere e aggiornare l'angolo di sterzata nel tempo.
 */

class CarInputController
{
public:

    // Comandi legati ai tasti 
    void setAccelerating(bool v) { inputState.isAccelerating = v; }
    void setBraking(bool v) { inputState.isBraking = v; }
    void setHandbrake(bool v) { inputState.isHandbrake = v; }
    void setSteeringLeft(bool v) { isSteeringLeft = v; }
    void setSteeringRight(bool v) { isSteeringRight = v; }

    // Comandi legati al mouse
    void setMouseSteering(bool active);
    void setMouseSteeringTarget(double angle);
    void setSteeringAngleDirect(double angle);

    // Aggiornamento frame 
    /**
     * @brief Aggiorna l'angolo di sterzata in base all'input e al tempo trascorso.
     * @param deltaTime Secondi dall'ultimo frame.
     */
    void updateSteeringAngle(double deltaTime);

    const CarInputState& getState() const { return inputState; }

private:
    CarInputState inputState;

    bool isSteeringLeft = false;
    bool isSteeringRight = false;

    bool   isMouseSteering = false;
    double mouseSteeringTarget = 0.0;

    // Costanti di sterzata
    static constexpr double MAX_STEERING_ANGLE = 35.0;  // Gradi
    static constexpr double STEERING_SPEED = 50.0;  // Gradi/s
};