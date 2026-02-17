/**
 * @file camera.h
 * @brief Definizione della gerarchia delle camere (Base, Prospettica, Ortografica).
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "node.h"

 /**
  * @class Camera
  * @brief Classe base che rappresenta una telecamera nella scena 3D.
  * * Estende la classe Node per ereditare le trasformazioni spaziali (posizione, rotazione).
  * Gestisce la matrice di proiezione e calcola la matrice di vista (View Matrix).
  */
class ENG_API Camera : public Node {
public:
    /**
     * @brief Costruttore della camera base.
     * @param name Il nome del nodo camera all'interno del grafo della scena.
     */
    Camera(const std::string& name);

    /**
     * @brief Distruttore virtuale.
     */
    virtual ~Camera();

    /**
     * @brief Restituisce la matrice di proiezione corrente.
     * @return Riferimento costante alla matrice 4x4 di proiezione.
     */
    const glm::mat4& getProjectionMatrix() const;

    /**
     * @brief Calcola e restituisce la View Matrix (inversa della World Matrix).
     * * La View Matrix trasforma le coordinate dal World Space al Camera Space.
     * @return Matrice 4x4 rappresentante la vista.
     */
    glm::mat4 getInvCameraMatrix() const;

    /**
     * @brief Imposta manualmente una matrice di proiezione generica.
     * @param projectionMatrix La nuova matrice di proiezione.
     */
    void setProjectionMatrix(const glm::mat4& projectionMatrix);

protected:
    /**
     * @brief Matrice di proiezione interna.
     */
    glm::mat4 projectionMatrix;
};
