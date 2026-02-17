#include "material.h"
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp> // Per glm::value_ptr



Material::Material(const std::string& name, const glm::vec3& emission, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, float transparency)
	: Object(name), emissione(emission), ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess), transparency(transparency), texture(nullptr)
{
}

Material::~Material() {}

// Getters
const glm::vec3& Material::getAmbient() const { return ambient; }
const glm::vec3& Material::getDiffuse() const { return diffuse; }
const glm::vec3& Material::getSpecular() const { return specular; }
const glm::vec3& Material::getEmissione() const { return emissione; }
float Material::getShininess() const { return shininess; }
float Material::getTransparency() const { return transparency; }
Texture* Material::getTexture() { return texture; }

// Setters
void Material::setAmbient(const glm::vec3& v) { ambient = v; }
void Material::setDiffuse(const glm::vec3& v) { diffuse = v; }
void Material::setSpecular(const glm::vec3& v) { specular = v; }
void Material::setEmissione(const glm::vec3& v) { emissione = v; }
void Material::setShininess(float v) { shininess = v; }
void Material::setTransparency(float v) { transparency = v; }
void Material::setTexture(Texture* t) { texture = t; }

void Material::render() {
	
	// Imposta parametri di shading Phong/Blinn-Phong
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(glm::vec4(ambient, transparency)));
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(glm::vec4(diffuse, transparency)));
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(glm::vec4(specular, transparency)));
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glm::value_ptr(glm::vec4(emissione, transparency)));
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	// Gestione Texture
	if (texture) {
        glEnable(GL_TEXTURE_2D);
        // Dice a OpenGL: "Moltiplica il colore della texture con il colore (e l'alpha) del materiale"
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 

        texture->render();
    } else {
        glDisable(GL_TEXTURE_2D);
    }
	
}