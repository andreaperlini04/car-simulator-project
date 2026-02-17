#pragma once
#include "camera.h"

/**
 * @class PerspectiveCamera
 * @brief Implementazione di una camera con proiezione prospettica.
 * * Simula la visione umana o fotografica, dove gli oggetti lontani appaiono più piccoli.
 */
class ENG_API PerspectiveCamera : public Camera {
public:
    /**
     * @brief Costruttore per la camera prospettica.
     */
    PerspectiveCamera(const std::string& name, float fovY, float aspect, float near, float far);

    /**
     * @brief Aggiorna il Field of View (FOV).
     * @param fovY Nuovo angolo FOV verticale in gradi.
     */
    void setFov(float fovY);

    /**
     * @brief Aggiorna l'aspect ratio.
     * @param width Larghezza della viewport.
     * @param height Altezza della viewport.
     */
    void setAspectRatio(float width, float height);

    /**
     * @brief Imposta la distanza del piano Near.
     * @param near Nuova distanza near.
     */
    void setNear(float near);

    /**
     * @brief Imposta la distanza del piano Far.
     * @param far Nuova distanza far.
     */
    void setFar(float far);

    /**
     * @brief Ricalcola la matrice di proiezione basandosi sui parametri correnti.
     * Viene chiamato automaticamente dai setter.
     */
    void updateProjection();

private:
    float fovY;   /// Campo visivo in gradi.
    float aspect; /// Rapporto larghezza/altezza.
    float zNear;  /// Piano Near.
    float zFar;   /// Piano Far.
};
