/**
 * @file node.h
 * @brief Header per la definizione dei nodi del grafo di scena.
 */
#pragma once
#include "object.h"
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "libConfig.h"

/**
 * @brief Rappresenta un nodo nel grafo della scena, gestisce trasformazioni e gerarchia.
 */
class ENG_API Node : public Object {
public:

   /**
   * @class Node
   * @brief Rappresenta un nodo nel grafo della scena, gestendo le trasformazioni locali e la gerarchia padre-figlio.
   */
   Node(const std::string& name);

   /**
   * @brief Distruttore virtuale di default.
   */
   virtual ~Node() = default;

   // Transformation matrix accessors

   /**
     * @brief Restituisce la matrice di trasformazione locale corrente.
     */
   glm::mat4 getM() const;

   /**
   * @brief Imposta la matrice di trasformazione locale del nodo.
   * @param newMatrix Nuova matrice 4x4 da assegnare.
   */
   void setM(const glm::mat4& newMatrix);

   // Transformation methods

   /**
     * @brief Applica una rotazione alla trasformazione locale corrente.
     * @param angle Angolo di rotazione in gradi.
     * @param axis Asse attorno al quale ruotare.
     */
   void rotate(float angle, const glm::vec3& axis);

   /**
     * @brief Applica un ridimensionamento alla trasformazione locale corrente.
     * @param factor Vettore contenente i fattori di scala per gli assi X, Y, Z.
     */
   void scale(const glm::vec3& factor);
   
   /**
     * @brief Applica una traslazione alla trasformazione locale corrente.
     * @param translation Vettore di spostamento.
     */
   void translate(const glm::vec3& translation);

   // World matrix computation
   /**
     * @brief Calcola la matrice di trasformazione globale moltiplicando la matrice locale con quelle dei nodi genitori.
     * @return Matrice che rappresenta la trasformazione del nodo nello spazio mondo (World Space).
     */
   glm::mat4 getWorldFinalMatrix() const;

   // Override render from Object
   /**
     * @brief Esegue il rendering del nodo e, ricorsivamente, dei suoi figli.
     */
   void render() override;

   // Parent-child hierarchy
   /**
     * @brief Restituisce il puntatore al nodo genitore.
     */
   Node* getParent() const;

   /**
     * @brief Imposta il nodo genitore per questo nodo.
     * @param newParent Puntatore al nuovo genitore.
     */
   void setParent(Node* newParent);

   /**
     * @brief Aggiunge un nodo figlio alla gerarchia di questo nodo.
     * @param child Puntatore al nodo da aggiungere.
     */
   void addChild(Node* child);

   /**
     * @brief Rimuove un nodo figlio specifico dalla lista dei figli.
     * @param child Puntatore al nodo da rimuovere.
     */
   void removeChild(Node* child);

   /**
     * @brief Recupera un nodo figlio tramite il suo indice.
     * @param index Indice posizionale del figlio nel vettore.
     */
   Node* getChild(int index) const;

   /**
     * @brief Restituisce il numero totale di figli collegati a questo nodo.
     */
   unsigned int getNumChildren() const;

   // Tree search
   /**
     * @brief Cerca ricorsivamente un nodo nella gerarchia tramite il suo nome.
     * @return Puntatore al nodo trovato o nullptr se non esiste.
     */
   Node* findByName(const std::string& nodeName);

private:
   /** @brief Matrice che definisce le trasformazioni geometriche locali rispetto al padre. */
   glm::mat4 transformationMatrix;
   /** @brief Collezione dei nodi figli dipendenti da questo nodo. */
   std::vector<Node*> children;
   /** @brief Riferimento al nodo genitore nella struttura gerarchica. */
   Node* parent;
};