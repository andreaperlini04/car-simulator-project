/**
 * @file mesh.h
 * @brief Definizione della classe Mesh per la gestione di geometrie poligonali.
 */
#pragma once
#include "node.h"
#include "material.h"
#include <vector>
#include <glm/glm.hpp>
#include "libConfig.h"

/**
* @class Mesh
* @brief Rappresenta un oggetto geometrico tridimensionale definito da vertici, facce e materiale.
* * Questa classe estende Node per gestire la geometria, le normali e le coordinate texture
* necessarie per il rendering.
*/
class ENG_API Mesh : public Node {
public:
      /**
      * @brief Inizializza una mesh vuota con il nome specificato.
      */
    Mesh(const std::string& name);

    /**
     * @brief Inizializza una mesh completa di geometria e materiale.
     * * @param matrix Matrice di trasformazione locale.
     * @param faces Numero totale di facce.
     * @param vertices Numero totale di vertici.
     * @param material Puntatore al materiale da applicare.
     */
    Mesh(const std::string& name, glm::mat4 matrix, unsigned int faces, unsigned int vertices, Material* material);

    // Getters

    /**
     * @brief Restituisce l'elenco dei vertici della mesh.
     */
    const std::vector<glm::vec3>& get_all_vertices() const;

    /**
     * @brief Restituisce l'elenco delle normali calcolate per i vertici.
     */
    const std::vector<glm::vec3>& get_all_normals() const;

    /**
     * @brief Restituisce le coordinate texture (UV) associate ai vertici.
     */
    const std::vector<glm::vec2>& get_all_texture_coords() const;

    /**
     * @brief Restituisce la struttura degli indici che definiscono le facce.
     */
    const std::vector<std::vector<unsigned int>>& get_face_vertices() const;

    /**
     * @brief Restituisce il puntatore al materiale corrente.
     */
    Material* getMaterial() const;

    // Setters
    /**
     * @brief Imposta i vertici che definiscono la geometria della mesh.
     */
    void set_all_vertices(const std::vector<glm::vec3>& vertices);

    /**
     * @brief Imposta i vettori normali per l'illuminazione.
     */
    void set_all_normals(const std::vector<glm::vec3>& normals);

    /**
     * @brief Imposta le coordinate per la mappatura delle texture.
     */
    void set_all_texture_coords(const std::vector<glm::vec2>& textureCoords);

    /**
     * @brief Definisce la topologia della mesh assegnando gli indici per ogni faccia.
     */
    void set_face_vertices(const std::vector<std::vector<unsigned int>>& faces);

    /**
     * @brief Associa un materiale alla mesh per il rendering.
     */
    void setMaterial(Material* material);

    /**
     * @brief Esegue il rendering della geometria.
     */
    void render() override;

protected:
   std::vector<glm::vec3> all_vertices;        /**< Vettore contenente le posizioni di tutti i vertici. */
   std::vector<glm::vec3> all_normals;         /**< Vettore delle normali per vertice. */
   std::vector<glm::vec2> all_texture_coords;  /**< Vettore delle coordinate texture UV. */
   std::vector<std::vector<unsigned int>> face_vertices; /**< Lista di indici che compongono le facce poligonali. */
   unsigned int numFaces;      /**< Conteggio totale delle facce. */
   unsigned int numVertices;   /**< Conteggio totale dei vertici. */
   Material* material;         /**< Puntatore al materiale associato alla mesh. */
   glm::mat4 matrix;           /**< Matrice di trasformazione locale specifica della mesh. */
};
