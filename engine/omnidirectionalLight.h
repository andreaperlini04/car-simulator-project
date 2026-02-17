/**
 * @file omnidirectionalLight.h
 * @brief Header per la definizione di sorgenti luminose omnidirezionali (puntiformi).
 */
#pragma once
#include "light.h"
#include <glm/glm.hpp>

 /**
  * @class OmnidirectionalLight
  * @brief Rappresenta una sorgente luminosa che emette luce uniformemente in tutte le direzioni.
  * * Simula una sorgente puntiforme (come una lampadina) che illumina la scena sfericamente.
  */
class ENG_API OmnidirectionalLight : public Light {
public:
   /**
    * @brief Costruttore di default.
    */
   OmnidirectionalLight();

   /**
    * @brief Inizializza una luce omnidirezionale con le proprietà luminose specificate.
    */
   OmnidirectionalLight(const std::string& name, const glm::mat4& matrix,
      const glm::vec3& ambient, const glm::vec3& diffuse,
      const glm::vec3& specular);

   /**
    * @brief Distruttore della classe.
    */
   ~OmnidirectionalLight();

   /**
    * @brief Restituisce l'angolo di apertura del fascio luminoso.
    * @return Angolo di cutoff (tipicamente 180.0f per luci omnidirezionali).
    */
   float getCutoff() const;

   /**
    * @brief Esegue il rendering della luce nella scena.
    */
   void render() override;

private:
   /** @brief Angolo massimo di illuminazione (180° indica copertura sferica completa). */
   float cutoff = 180.0f; // 180° = omnidirezionale
};