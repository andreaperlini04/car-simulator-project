/**
 * @file infiniteLight.h
 * @brief Definizione della classe per la gestione di luci direzionali (infinite).
 */
#pragma once
#include "light.h"
#include <glm/glm.hpp>

 /**
  * @class InfiniteLight
  * @brief Rappresenta una sorgente luminosa direzionale posta a distanza infinita.
  * * Questa classe simula una luce i cui raggi colpiscono gli oggetti parallelamente,
  * ideale per simulare sorgenti come il sole.
  */
class ENG_API InfiniteLight : public Light {
public:
      /**
      * @brief Costruttore di default.
      */
    InfiniteLight();

    /**
     * @brief Inizializza una luce infinita specificando le componenti luminose e la direzione.
     * * @param direction Vettore che definisce la direzione di propagazione della luce.
     */
    InfiniteLight(const std::string& name, const glm::mat4& matrix,
        const glm::vec3& ambient, const glm::vec3& diffuse,
        const glm::vec3& specular, const glm::vec3& direction);

    /**
     * @brief Distruttore della classe.
     */
    ~InfiniteLight();

    /**
     * @brief Imposta la direzione di emissione della luce.
     */
    void setDirection(const glm::vec3& dir);

    /**
     * @brief Restituisce il vettore direzione corrente della luce.
     */
    glm::vec3 getDirection() const;

    /**
     * @brief Esegue il rendering della luce all'interno della scena.
     */
    void render() override;
};