#include "omnidirectionalLight.h"
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

OmnidirectionalLight::OmnidirectionalLight() : Light()
{
}

OmnidirectionalLight::OmnidirectionalLight(const std::string& name, const glm::mat4& matrix,
    const glm::vec3& ambient, const glm::vec3& diffuse,
    const glm::vec3& specular)
    : Light(name, matrix)
{
    setAmbient(glm::vec4(ambient, 1.0f));
    setDiffuse(glm::vec4(diffuse, 1.0f));
    setSpecular(glm::vec4(specular, 1.0f));

    // Estrai la posizione dalla matrice di trasformazione
    glm::vec3 position(matrix[3][0], matrix[3][1], matrix[3][2]);
    setPosition(glm::vec4(position, 1.0f)); // w=1.0 per luce posizionale
}

OmnidirectionalLight::~OmnidirectionalLight()
{
}

float OmnidirectionalLight::getCutoff() const
{
    return cutoff;
}

void OmnidirectionalLight::render()
{
    Light::render();
    

    // Usiamo (0,0,0,1) perché la ModelView Matrix (caricata dalla List)
    // ha già spostato il "cursore" nel punto esatto della luce.
    // se usassimo glm::value_ptr(position) posizione sarebbe currentMatrix * position
    GLfloat zeroPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(lightContextID, GL_POSITION, zeroPos);

    glLightf(lightContextID, GL_SPOT_CUTOFF, cutoff); // 180.0 per omni

    // Attenuazione standard
    glLightf(lightContextID, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(lightContextID, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(lightContextID, GL_QUADRATIC_ATTENUATION, 0.0f);
}