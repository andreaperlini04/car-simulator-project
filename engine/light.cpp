#include "light.h"
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

Light::Light() : Node("Light"),
lightContextID(-1),
ambient(0.0f),
diffuse(1.0f),
specular(1.0f),
position(0.0f, 0.0f, 0.0f, 1.0f)
{

}

Light::Light(const std::string& name, const glm::mat4& matrix)
    : Node(name),
    lightContextID(-1),
    ambient(0.0f),
    diffuse(1.0f),
    specular(1.0f),
    position(0.0f, 0.0f, 0.0f, 1.0f)
{
   this->setM(matrix);
}

Light::~Light() {}

const glm::vec4& Light::getAmbient() const { return ambient; }
const glm::vec4& Light::getDiffuse() const { return diffuse; }
const glm::vec4& Light::getSpecular() const { return specular; }
const glm::vec4& Light::getPosition() const { return position; }

void Light::setAmbient(const glm::vec4& v) { ambient = v; }
void Light::setDiffuse(const glm::vec4& v) { diffuse = v; }
void Light::setSpecular(const glm::vec4& v) { specular = v; }
void Light::setPosition(const glm::vec4& v) { position = v; }

void Light::setLightID(int id) {
    this->lightContextID = id;
}

void Light::render() {
    if (lightContextID < 0) return;
    glEnable(lightContextID);
    glLightfv(lightContextID, GL_AMBIENT, glm::value_ptr(ambient));
    glLightfv(lightContextID, GL_DIFFUSE, glm::value_ptr(diffuse));
    glLightfv(lightContextID, GL_SPECULAR, glm::value_ptr(specular));
}