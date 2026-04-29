#include "node.h"
#include <algorithm> // Necessario per std::remove/find_if

ENG_API Node::Node(const std::string& name)
   : Object( name), parent(nullptr), transformationMatrix(glm::mat4(1.0f))
{
}

glm::mat4 Node::getM() const {
   return transformationMatrix;
}

void Node::setM(const glm::mat4& newMatrix) {
   transformationMatrix = newMatrix;
}

// Trasformazioni (aggiornano la matrice locale)
void Node::rotate(float angle, const glm::vec3& axis) {
   transformationMatrix = glm::rotate(transformationMatrix, glm::radians(angle), axis);
}

void Node::scale(const glm::vec3& factor) {
   transformationMatrix = glm::scale(transformationMatrix, factor);
}

void Node::translate(const glm::vec3& translation) {
   transformationMatrix = glm::translate(transformationMatrix, translation);
}

// Calcolo ricorsivo della matrice mondo (World Matrix)
glm::mat4 Node::getWorldFinalMatrix() const {
   if (parent) {
      // Moltiplica la matrice del genitore per quella locale
      return parent->getWorldFinalMatrix() * transformationMatrix;
   }
   return transformationMatrix;
}

void Node::render() {
   // Metodo base vuoto, verrà sovrascritto dalle classi derivate (Mesh, Light, ecc.)
}

// --- Gestione Gerarchia ---

Node* Node::getParent() const { return parent; }

void Node::setParent(Node* newParent) { parent = newParent; }

void Node::addChild(Node* child) {
   if (child) {
      child->setParent(this);
      children.push_back(std::unique_ptr<Node>(child)); // Prende possesso qui
   }
}

Node* Node::removeChild(Node* child) {
   auto it = std::find_if(children.begin(), children.end(), [child](const std::unique_ptr<Node>& ptr) {
      return ptr.get() == child;
      });
   if (it != children.end()) {
      child->setParent(nullptr);
      Node* released = it->release(); // Rilascia l'ownership dal unique_ptr interno senza distruggere l'oggetto
      children.erase(it);
      return released;
   }
   return nullptr;
}

Node* Node::getChild(int index) const {
   if (index >= 0 && index < children.size())
      return children[index].get();
   return nullptr;
}

unsigned int Node::getNumChildren() const {
   return (unsigned int)children.size();
}

// Ricerca ricorsiva per nome
Node* Node::findByName(const std::string& nodeName) {
   if (this->name == nodeName) return this;

   for (const auto& child : children) {
      Node* res = child->findByName(nodeName);
      if (res) return res;
   }
   return nullptr;
}