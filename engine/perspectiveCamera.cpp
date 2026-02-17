#include "perspectiveCamera.h"
/**
 * @file camera.cpp
 * @brief Implementazione delle classe perspectiveCamera.
 */

PerspectiveCamera::PerspectiveCamera(const std::string& name, float fovY, float aspect, float near, float far)
    : Camera(name), fovY(fovY), aspect(aspect), zNear(near), zFar(far)
{
    updateProjection();
}

void PerspectiveCamera::setFov(float fov) {
    this->fovY = fov;
    updateProjection();
}

void PerspectiveCamera::setAspectRatio(float width, float height) {
    // Evita divisioni per zero se la finestra è minimizzata
    if (height <= 0.0f) height = 1.0f;

    this->aspect = width / height;
    updateProjection();
}

void PerspectiveCamera::setNear(float near) {
    this->zNear = near;
    updateProjection();
}

void PerspectiveCamera::setFar(float far) {
    this->zFar = far;
    updateProjection();
}

void PerspectiveCamera::updateProjection() {
    // glm::perspective richiede il FOV in radianti
    this->projectionMatrix = glm::perspective(glm::radians(fovY), aspect, zNear, zFar);
}