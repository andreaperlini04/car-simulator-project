#include "list.h"
#include "light.h"
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "mesh.h"

ENG_API List::List() : Object("RenderList") {}
List::~List() { clear(); }

void List::pass(Node* node, glm::mat4 parentMatrix) {
   if (!node) return;

   // Calcola matrice mondo
   glm::mat4 currentWorldMatrix = parentMatrix * node->getM();

   // Aggiungi istanza
   Instance inst;
   inst.node = node;
   inst.nodeWorldMatrix = currentWorldMatrix;

   // Se è una luce, la mettiamo in testa alla lista per elaborarla prima
   if (dynamic_cast<Light*>(node) != nullptr)
      instances.push_front(inst);
   else
      instances.push_back(inst);

   // Ricorsione
   for (unsigned int i = 0; i < node->getNumChildren(); i++) {
      pass(node->getChild(i), currentWorldMatrix);
   }
}

void List::render(glm::mat4 viewMatrix) {
   int lightCounter = 0;
   const int MAX_HARDWARE_LIGHTS = 8;
   std::list<Instance> transp;

   // Spegni tutte le luci per sicurezza all'inizio del frame
   for (int i = 0; i < MAX_HARDWARE_LIGHTS; i++) glDisable(GL_LIGHT0 + i);

   for (auto& inst : instances) {
      // Calcola ModelView = View * World
      glm::mat4 modelView = viewMatrix * inst.nodeWorldMatrix;
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf(glm::value_ptr(modelView));

      // Controllo se è una luce
      Light* lightNode = dynamic_cast<Light*>(inst.node);

      if (lightNode) {
         if (lightCounter < MAX_HARDWARE_LIGHTS) {
            // Assegna slot hardware e renderizza
            lightNode->setLightID(GL_LIGHT0 + lightCounter);
            lightNode->render();
            lightCounter++;
         }
         else {
            // Limite superato
            std::cout << "NUMERO MAX RAGGIUNTO: Luce '" << lightNode->getName() << "' ignorata." << std::endl;
            lightNode->setLightID(-1); // Disabilita
         }
      }
      else {
         
         Mesh* mesh = dynamic_cast<Mesh*>(inst.node);
         if (mesh) {
            // Se ha un materiale e la trasparenza è < 1.0 (es. scacchiera 0.8)
            if (mesh->getMaterial() && mesh->getMaterial()->getTransparency() < 1.0f) {
               transp.push_back(inst);
            }
            else {
               inst.node->render();
            }
         }
         else {
            
            inst.node->render();
         }

         

         
      }

   }
   
   for (auto& inst : transp) {
      glm::mat4 modelView = viewMatrix * inst.nodeWorldMatrix;
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf(glm::value_ptr(modelView));
      glEnable(GL_BLEND);
      
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask(GL_FALSE);
      glDisable(GL_CULL_FACE); // Renderizza anche il retro delle facce trasparenti

      inst.node->render();

      glEnable(GL_CULL_FACE);
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
   }
   
   


}

void List::render() {
   // Fallback: se chiamato senza matrice vista, usa l'identità (nessuna trasformazione camera)
   render(glm::mat4(1.0f));
}

void List::clear() {
   instances.clear();
}