#include "camera.h"
#include <glm/gtc/matrix_transform.hpp> 

/**
 * @file camera.cpp
 * @brief Implementazione delle classe Camera.
 */
Camera::Camera(const std::string& name) : Node(name), projectionMatrix(glm::mat4(1.0f)) {}

Camera::~Camera() {}

const glm::mat4& Camera::getProjectionMatrix() const {
   return projectionMatrix;
}

void Camera::setProjectionMatrix(const glm::mat4& projectionMatrix) {
   this->projectionMatrix = projectionMatrix;
}

glm::mat4 Camera::getInvCameraMatrix() const {
    // Ottiene la trasformazione finale del nodo nel mondo (World Matrix)
     // L'inversa di questa matrice sposta il mondo rispetto alla camera (View Matrix)
   return glm::inverse(this->getWorldFinalMatrix());
}
