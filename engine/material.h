/**
 * @file material.h
 * @brief Header per la definizione delle proprietà dei materiali di rendering.
 */
#pragma once
#include "object.h"
#include "texture.h"
#include "glm/glm.hpp"
#include "libConfig.h"

/**
* @class Material
* @brief Rappresenta le proprietà ottiche e superficiali di un oggetto grafico.
* * Definisce come l'oggetto interagisce con la luce (modello di Phong) e l'eventuale texture applicata.
*/
class ENG_API Material : public Object {
public:
	/**
	 * @brief Inizializza un nuovo materiale con le proprietà specificate.
	 * @param emission Colore emesso dal materiale (auto-illuminazione).
	 * @param ambient Coefficiente di riflessione della luce ambientale.
	 * @param diffuse Coefficiente di riflessione diffusa.
	 * @param specular Coefficiente di riflessione speculare.
	 * @param shininess Fattore di brillantezza (esponente speculare).
	 * @param transparency Livello di trasparenza del materiale.
	 */
	Material(const std::string& name, const glm::vec3& emission, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, float transparency);

	/**
	 * @brief Distruttore della classe.
	 */
	~Material();
	// Getters

	/**
	 * @brief Restituisce la componente di riflessione ambientale.
	 */
	const glm::vec3& getAmbient() const;

	/**
	 * @brief Restituisce la componente di riflessione diffusa.
	 */
	const glm::vec3& getDiffuse() const;

	/**
	 * @brief Restituisce la componente di riflessione speculare.
	 */
	const glm::vec3& getSpecular() const;

	/**
	 * @brief Restituisce la componente emissiva del materiale.
	 */
	const glm::vec3& getEmissione() const;

	/**
	 * @brief Restituisce il fattore di brillantezza (shininess).
	 */
	float getShininess() const;

	/**
	 * @brief Restituisce il valore di trasparenza corrente.
	 */
	float getTransparency() const;

	/**
	 * @brief Restituisce il puntatore alla texture associata, se presente.
	 */
	Texture* getTexture();

	// Setters

	/**
	 * @brief Imposta la componente di riflessione ambientale.
	 */
	void setAmbient(const glm::vec3& ambient);

	/**
	 * @brief Imposta la componente di riflessione diffusa.
	 */
	void setDiffuse(const glm::vec3& diffuse);

	/**
	 * @brief Imposta la componente di riflessione speculare.
	 */
	void setSpecular(const glm::vec3& specular);

	/**
	 * @brief Imposta la componente emissiva (auto-illuminazione).
	 */
	void setEmissione(const glm::vec3& emissione);

	/**
	 * @brief Imposta il fattore di brillantezza per i riflessi speculari.
	 */
	void setShininess(float shininess);

	/**
	 * @brief Imposta il livello di trasparenza del materiale.
	 */
	void setTransparency(float transparency);

	/**
	 * @brief Associa una texture al materiale.
	 * @param texture Puntatore alla texture da applicare.
	 */
	void setTexture(Texture* texture);

	/**
	 * @brief Applica le proprietà del materiale allo stato corrente del rendering.
	 */
	void render() override;
private:
	/** @brief Colore riflesso sotto luce ambientale. */
	glm::vec3 ambient;
	/** @brief Colore riflesso sotto luce diffusa. */
	glm::vec3 diffuse;
	/** @brief Colore del riflesso speculare (highlight). */
	glm::vec3 specular;
	/** @brief Colore emesso spontaneamente dall'oggetto. */
	glm::vec3 emissione;
	/** @brief Esponente per il calcolo della dimensione del riflesso speculare. */
	float shininess;
	/** @brief Valore che determina l'opacità o trasparenza dell'oggetto. */
	float transparency;
	/** @brief Puntatore alla texture applicata alla superficie. */
	Texture* texture;
};