#include "spotLight.h"
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

SpotLight::SpotLight() : Light(), spotExponent(0.0f)
{
}

SpotLight::SpotLight(const std::string& name, const glm::mat4& matrix,
   const glm::vec3& ambient, const glm::vec3& diffuse,
   const glm::vec3& specular, const glm::vec3& dir,
   float cutoff, float exponent) 
   : Light(name, matrix), spotExponent(exponent) 
{
   setAmbient(glm::vec4(ambient, 1.0f));
   setDiffuse(glm::vec4(diffuse, 1.0f));
   setSpecular(glm::vec4(specular, 1.0f));

   setCutoff(cutoff);

   // Estrai la posizione dalla matrice
   glm::vec3 position(matrix[3][0], matrix[3][1], matrix[3][2]);
   setPosition(glm::vec4(position, 1.0f));

   setDirection(dir);
}

SpotLight::~SpotLight()
{
}

float SpotLight::getCutoff() const
{
    return cutoff;
}

void SpotLight::setCutoff(float value)
{
    cutoff = value;
}

void SpotLight::setDirection(const glm::vec3& dir)
{
    direction = glm::normalize(dir);
}

glm::vec3 SpotLight::getDirection() const
{
    return direction;
}

void SpotLight::setSpotExponent(float exp)
{
    spotExponent = exp;
}

float SpotLight::getSpotExponent() const
{
    return spotExponent;
}

void SpotLight::render()
{
   Light::render();

   // Usiamo (0,0,0,1) perché la ModelView Matrix (caricata dalla List)
   // ha già spostato il "cursore" nel punto esatto della luce.
   // se usassimo glm::value_ptr(position) sarebbe più in alto
   GLfloat zeroPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
   glLightfv(lightContextID, GL_POSITION, zeroPos);

   
   
   glLightfv(lightContextID, GL_SPOT_DIRECTION, glm::value_ptr(direction));

   // Parametri Spot
   glLightf(lightContextID, GL_SPOT_CUTOFF, cutoff);

   glLightf(lightContextID, GL_SPOT_EXPONENT, 0.0f);

   // Attenuazione (standard)
   glLightf(lightContextID, GL_CONSTANT_ATTENUATION, 1.0f);
   glLightf(lightContextID, GL_LINEAR_ATTENUATION, 0.0f);
   glLightf(lightContextID, GL_QUADRATIC_ATTENUATION, 0.0f);
}