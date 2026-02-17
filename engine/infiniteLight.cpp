#include "infiniteLight.h"
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

InfiniteLight::InfiniteLight() : Light()
{
    setPosition(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
}

InfiniteLight::InfiniteLight(const std::string& name, const glm::mat4& matrix,
    const glm::vec3& ambient, const glm::vec3& diffuse,
    const glm::vec3& specular, const glm::vec3& direction)
    : Light(name, matrix)
{
    setAmbient(glm::vec4(ambient, 1.0f));
    setDiffuse(glm::vec4(diffuse, 1.0f));
    setSpecular(glm::vec4(specular, 1.0f));
    setDirection(direction);
}

InfiniteLight::~InfiniteLight()
{
}

void InfiniteLight::setDirection(const glm::vec3& dir) {
    // w=0.0 => luce direzionale
    glm::vec3 normalized = glm::normalize(dir);
    setPosition(glm::vec4(normalized.x, normalized.y, normalized.z, 0.0f));
}

glm::vec3 InfiniteLight::getDirection() const {
    glm::vec4 pos = getPosition();
    return glm::vec3(pos.x, pos.y, pos.z);
}

void InfiniteLight::render() {
    Light::render();

    // Luce direzionale: w=0 nel vettore posizione
    glLightfv(lightContextID, GL_POSITION, glm::value_ptr(getPosition()));
}