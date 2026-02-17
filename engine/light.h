/**
 * @file light.h
 * @brief Header per la definizione della classe base delle sorgenti luminose.
 */
#pragma once
#include "node.h"
#include "libConfig.h"
#include <glm/glm.hpp>

 /**
  * @class Light
  * @brief Classe base astratta che rappresenta una generica sorgente luminosa nel grafo di scena.
  * * Gestisce le componenti cromatiche fondamentali (ambientale, diffusa, speculare) e la posizione.
  */
class ENG_API Light : public Node
{
public:
   /**
    * @brief Costruttore di default.
    */
   Light();

   /**
    * @brief Inizializza una luce con nome e trasformazione iniziale.
    * @param name Nome del nodo luce.
    * @param matrix Matrice di trasformazione iniziale.
    */
   Light(const std::string& name, const glm::mat4& matrix);

   /**
    * @brief Distruttore virtuale puro.
    */
   virtual ~Light() = 0;

   /**
    * @brief Restituisce la componente di luce ambientale.
    */
   const glm::vec4& getAmbient() const;

   /**
    * @brief Restituisce la componente di luce diffusa.
    */
   const glm::vec4& getDiffuse() const;

   /**
    * @brief Restituisce la componente di luce speculare.
    */
   const glm::vec4& getSpecular() const;

   /**
    * @brief Restituisce la posizione della sorgente luminosa.
    */
   const glm::vec4& getPosition() const;

   /**
    * @brief Imposta la componente di luce ambientale.
    */
   void setAmbient(const glm::vec4& ambient);

   /**
    * @brief Imposta la componente di luce diffusa.
    */
   void setDiffuse(const glm::vec4& diffuse);

   /**
    * @brief Imposta la componente di luce speculare.
    */
   void setSpecular(const glm::vec4& specular);

   /**
    * @brief Imposta la posizione della sorgente luminosa.
    */
   void setPosition(const glm::vec4& position);

   /**
    * @brief Assegna l'identificativo numerico della luce nel contesto grafico (es. GL_LIGHTi).
    * @param id Indice della luce da attivare.
    */
   void setLightID(int id);

   /**
    * @brief Attiva e configura la luce nella pipeline di rendering.
    */
   virtual void render() override;

protected:
   /** @brief Intensità e colore della componente ambientale. */
   glm::vec4 ambient;
   /** @brief Intensità e colore della componente diffusa. */
   glm::vec4 diffuse;
   /** @brief Intensità e colore della componente speculare. */
   glm::vec4 specular;
   /** @brief Coordinate spaziali della sorgente luminosa (o direzione per luci direzionali). */
   glm::vec4 position;
   /** @brief ID hardware o del contesto grafico assegnato a questa luce. */
   int lightContextID;
};