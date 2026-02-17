/**
 * @file spotLight.h
 * @brief Header per la definizione di sorgenti luminose di tipo spot (faretto).
 */
#pragma once
#include "light.h"
#include <glm/glm.hpp>

 /**
  * @class SpotLight
  * @brief Rappresenta una sorgente luminosa che emette luce in un cono specifico (faretto).
  * * Simula una luce posizionale con una direzione e un'apertura angolare limitata.
  */
class ENG_API SpotLight : public Light {
public:
   /**
     * @brief Costruttore di default.
     */
    SpotLight();

    /**
     * @brief Inizializza una luce spot con proprietà e orientamento specifici.
     * @param cutoff Angolo di apertura del cono di luce.
     * @param exponent Fattore di attenuazione angolare della luce all'interno del cono.
     */
    SpotLight(const std::string& name, const glm::mat4& matrix,
       const glm::vec3& ambient, const glm::vec3& diffuse,
       const glm::vec3& specular, const glm::vec3& direction,
       float cutoff, float exponent);

    /**
     * @brief Distruttore della classe.
     */
    ~SpotLight();

    /**
     * @brief Restituisce l'angolo di apertura del cono (cutoff).
     */
    float getCutoff() const;

    /**
     * @brief Imposta l'angolo limite del cono di emissione.
     */
    void setCutoff(float value);

    /**
     * @brief Imposta la direzione verso cui punta il faretto.
     */
    void setDirection(const glm::vec3& dir);

    /**
     * @brief Restituisce il vettore direzione corrente.
     */
    glm::vec3 getDirection() const;

    /**
     * @brief Imposta l'esponente di attenuazione angolare (spot exponent).
     * @param exp Valore che determina quanto la luce è concentrata al centro del fascio.
     */
    void setSpotExponent(float exp);

    /**
     * @brief Restituisce il fattore di attenuazione angolare corrente.
     */
    float getSpotExponent() const;

    /**
     * @brief Esegue il rendering della luce spot nella scena.
     */
    void render() override;

private:
   /** @brief Angolo che definisce l'ampiezza massima del cono di luce. */
   float cutoff;
   /** @brief Fattore che controlla la distribuzione dell'intensità dal centro ai bordi del cono. */
   float spotExponent = 0.0f;
   /** @brief Vettore che indica la direzione di emissione della luce. */
   glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
};