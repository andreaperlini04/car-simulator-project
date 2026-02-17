/**
 * @file texture.h
 * @brief Header per la gestione delle texture 2D.
 */
#pragma once
#include "object.h"
#include "libConfig.h"
#include <string>

 /**
  * @class Texture
  * @brief Rappresenta una texture 2D applicabile alle superfici degli oggetti grafici.
  */
class ENG_API Texture : public Object {
public:
	/**
	 * @brief Carica e inizializza una texture da file.
	 * @param filepath Percorso del file immagine da caricare.
	 */
	Texture(const std::string& name, const std::string& filepath); // Updated constructor to take filepath

	/**
	 * @brief Distruttore della classe.
	 */
	~Texture();

	/**
	 * @brief Attiva la texture per l'uso nel rendering corrente.
	 */
	void render() override;
private:
	/** @brief Percorso del file sorgente dell'immagine. */
	std::string m_filepath;
	/** @brief Identificativo numerico interno della texture (handle). */
	unsigned int m_texId;
};