/**
 * @file list.h
 * @brief Header per la gestione delle liste di rendering.
 */
#pragma once
#include "object.h"
#include "node.h"
#include <list>
#include "libConfig.h"

/**
* @class List
* @brief Gestisce una collezione di nodi grafici da renderizzare in un determinato passaggio.
*/
class ENG_API List : public Object {
public:
	/**
	 * @brief Costruttore di default.
	 */
	List();

	/**
	 * @brief Distruttore della classe.
	 */
	~List();

	/**
	 * @brief Aggiunge un nodo alla lista di rendering con la relativa trasformazione.
	 * @param node Puntatore al nodo da inserire in lista.
	 * @param mat Matrice di trasformazione mondiale (World Matrix) associata al nodo.
	 */
	void pass(Node* node, glm::mat4 mat);

	/**
	 * @brief Esegue il rendering di tutti gli elementi contenuti nella lista.
	 * @param viewMatrix Matrice di vista corrente utilizzata per il rendering.
	 */
	void render(glm::mat4 viewMatrix);

	/**
	 * @brief Implementazione del metodo di rendering generico (ereditato da Object).
	 */
	void render() override;

	/**
	 * @brief Rimuove tutti gli elementi presenti nella lista di rendering.
	 */
	void clear();

protected:
	/**
	 * @struct Instance
	 * @brief Rappresenta una singola istanza di rendering contenente il nodo e la sua posizione.
	 */
	struct Instance {

		/** @brief Puntatore al nodo grafico. */
		Node* node;

		/** @brief Matrice di trasformazione dell'oggetto nello spazio mondo. */
		glm::mat4 nodeWorldMatrix;
	};

	/** @brief Contenitore interno delle istanze da elaborare. */
	std::list<Instance> instances;
};


