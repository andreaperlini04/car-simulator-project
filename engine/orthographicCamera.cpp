#include "orthographicCamera.h"
/**
 * @file camera.cpp
 * @brief Implementazione delle classe orthographicCamera.
 */

OrthographicCamera::OrthographicCamera(const std::string& name, float left, float right, float bottom, float top, float near, float far)
    : Camera(name), left(left), right(right), bottom(bottom), top(top), zNear(near), zFar(far)
{
    updateProjection();
}

void OrthographicCamera::setFrustum(float l, float r, float b, float t, float n, float f) {
    left = l;
    right = r;
    bottom = b;
    top = t;
    zNear = n;
    zFar = f;

    updateProjection();
}

void OrthographicCamera::updateProjection() {
    this->projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
}