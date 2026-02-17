/**
 * @file object.h
 * @brief Header per la definizione della classe base degli oggetti di scena.
 */

#pragma once
#include <string>
#include <memory>
#include <atomic>
#include "libConfig.h"

 /**
  * @class Object
  * @brief Classe base astratta per tutti gli oggetti della scena.
  * * Gestisce l'identificazione univoca delle istanze e definisce l'interfaccia comune per il rendering.
  */
class ENG_API Object {
private:
   /** @brief Contatore statico atomico per la generazione sequenziale degli ID univoci. */
   static std::atomic<unsigned int> nextId;
public:
   /**
    * @brief Costruttore che inizializza l'oggetto con un nome e gli assegna un ID univoco.
    * @param name Nome identificativo da assegnare all'oggetto.
    */
   Object(const std::string& name);

   /**
    * @brief Metodo puramente virtuale per il rendering dell'oggetto.
    * * Deve essere implementato dalle classi derivate per definire la logica di disegno specifica.
    */
   virtual void render() = 0;

   /**
    * @brief Restituisce il nome assegnato all'oggetto.
    */
   virtual std::string getName() const;

   /**
    * @brief Restituisce l'identificativo numerico univoco dell'oggetto.
    */
   virtual unsigned int getId() const;

   /**
    * @brief Distruttore virtuale di default.
    */
   virtual ~Object() = default;

protected:
   /** @brief Identificativo numerico univoco dell'istanza. */
   unsigned int uid;
   /** @brief Nome identificativo dell'oggetto. */
   std::string name;
};