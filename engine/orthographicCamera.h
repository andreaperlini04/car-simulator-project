#pragma once
#include "camera.h"
/**
 * @class OrthographicCamera
 * @brief Implementazione di una camera con proiezione ortografica.
 */
class ENG_API OrthographicCamera : public Camera {
public:
    /**
     * @brief Costruttore per la camera ortografica.
     */
    OrthographicCamera(const std::string& name, float left, float right, float bottom, float top, float near, float far);

    /**
     * @brief Aggiorna le dimensioni del frustum ortografico.
     * @param left Sinistra.
     * @param right Destra.
     * @param bottom Sotto.
     * @param top Sopra.
     * @param near Vicino.
     * @param far Lontano.
     */
    void setFrustum(float left, float right, float bottom, float top, float near, float far);

    /**
     * @brief Ricalcola la matrice ortografica.
     */
    void updateProjection();

private:
    float left, right, bottom, top, zNear, zFar;
};
