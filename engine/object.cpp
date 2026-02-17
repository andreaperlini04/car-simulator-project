#include "object.h"

// Inizializzazione
std::atomic<unsigned int> Object::nextId{0};

ENG_API Object::Object(const std::string& name)
   : name(name)
{
   // Assegna il valore corrente e poi lo incrementa per il prossimo oggetto
   this->uid = nextId++;
}

std::string Object::getName() const {
   return this->name;
}

unsigned int Object::getId() const {
   return this->uid;
}