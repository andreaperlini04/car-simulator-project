/**
 * @file engine_test.cpp
 * @brief Test suite per il motore grafico (Standalone)
 *
 */

#include <assert.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

 // GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Engine includes
#include "object.h"
#include "node.h"
#include "camera.h"
#include "mesh.h"
#include "material.h"
#include "light.h"
#include "omnidirectionalLight.h"
#include "infiniteLight.h"
#include "spotLight.h"
#include "list.h"

// Macro di utilità per il confronto float
#define EPSILON 0.0001f
bool areVec3Equal(const glm::vec3& a, const glm::vec3& b) {
   return glm::length(a - b) < EPSILON;
}

int main() {
   std::cout << "==========================================" << std::endl;
   std::cout << "      AVVIO ENGINE TEST SUITE (MAIN)      " << std::endl;
   std::cout << "==========================================" << std::endl;

   // ------------------------------------------------------------------------
   // 1. TESTING OBJECT & NODE (Base)
   // ------------------------------------------------------------------------
   std::cout << "[TEST] Object & Node Base... ";

   Node* node1 = new Node("NodoTest1");

   // Test Nome
   assert(node1->getName() == "NodoTest1");

   // Test ID (L'ID è atomico e incrementale)
   unsigned int id1 = node1->getId();
   Node* node2 = new Node("NodoTest2");
   unsigned int id2 = node2->getId();
   assert(id1 != id2);
   assert(id2 > id1);

   // Test Matrice Identità Iniziale
   glm::mat4 m = node1->getM();
   assert(m == glm::mat4(1.0f));

   std::cout << "OK" << std::endl;

   // ------------------------------------------------------------------------
   // 2. TESTING NODE TRANSFORMATIONS & HIERARCHY
   // ------------------------------------------------------------------------
   std::cout << "[TEST] Node Transformations & Hierarchy... ";

   Node* root = new Node("Root");
   Node* child = new Node("Child");

   // Test Gerarchia
   root->addChild(child);
   assert(root->getNumChildren() == 1);
   assert(root->getChild(0) == child);
   assert(child->getParent() == root);

   // Test Trasformazioni e World Matrix
   // Spostiamo la root a (10, 0, 0)
   root->translate(glm::vec3(10.0f, 0.0f, 0.0f));

   // Spostiamo il figlio localmente a (0, 5, 0)
   child->translate(glm::vec3(0.0f, 5.0f, 0.0f));

   // La posizione nel mondo del figlio dovrebbe essere (10, 5, 0)
   glm::mat4 worldMat = child->getWorldFinalMatrix();
   glm::vec3 worldPos(worldMat[3]); // La colonna 3 è la posizione

   assert(areVec3Equal(worldPos, glm::vec3(10.0f, 5.0f, 0.0f)));

   // Test Rimozione Figlio
   root->removeChild(child);
   assert(root->getNumChildren() == 0);
   assert(child->getParent() == nullptr);

   std::cout << "OK" << std::endl;

   // ------------------------------------------------------------------------
   // 3. TESTING CAMERA
   // ------------------------------------------------------------------------
   std::cout << "[TEST] Camera... ";

   Camera* cam = new Camera("MainCam");

   // Test Proiezione (Perspective)
   glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.33f, 0.1f, 100.0f);
   cam->setProjectionMatrix(proj);
   assert(cam->getProjectionMatrix() == proj);

   // Test View Matrix (Inversa della World Matrix)
   cam->translate(glm::vec3(0.0f, 0.0f, 10.0f)); // Camera a Z=10
   glm::mat4 viewMat = cam->getInvCameraMatrix();

   // Se la camera è a +10, la view matrix deve traslare il mondo a -10
   glm::vec3 viewTranslate(viewMat[3]);
   assert(areVec3Equal(viewTranslate, glm::vec3(0.0f, 0.0f, -10.0f)));

   std::cout << "OK" << std::endl;

   // ------------------------------------------------------------------------
   // 4. TESTING MATERIAL
   // ------------------------------------------------------------------------
   std::cout << "[TEST] Material... ";

   glm::vec3 ambient(0.2f);
   glm::vec3 diffuse(0.8f, 0.0f, 0.0f); // Rosso
   glm::vec3 specular(1.0f);
   glm::vec3 emission(0.0f);
   float shininess = 32.0f;
   float transparency = 0.5f;

   Material* mat = new Material("RossoSemiTrasp", emission, ambient, diffuse, specular, shininess, transparency);

   assert(areVec3Equal(mat->getDiffuse(), diffuse));
   assert(mat->getTransparency() == 0.5f);
   assert(mat->getShininess() == 32.0f);

   // Test setter
   mat->setTransparency(1.0f);
   assert(mat->getTransparency() == 1.0f);

   std::cout << "OK" << std::endl;

   // ------------------------------------------------------------------------
   // 5. TESTING MESH
   // ------------------------------------------------------------------------
   std::cout << "[TEST] Mesh... ";

   Mesh* mesh = new Mesh("Triangolo");
   mesh->setMaterial(mat);

   assert(mesh->getMaterial() == mat);
   assert(mesh->getName() == "Triangolo");

   // Test Geometry Data
   std::vector<glm::vec3> verts = { {0,0,0}, {1,0,0}, {0,1,0} };
   std::vector<glm::vec3> norms = { {0,0,1}, {0,0,1}, {0,0,1} };

   mesh->set_all_vertices(verts);
   mesh->set_all_normals(norms);

   assert(mesh->get_all_vertices().size() == 3);
   assert(mesh->get_all_normals().size() == 3);
   assert(areVec3Equal(mesh->get_all_vertices()[1], glm::vec3(1, 0, 0)));

   std::cout << "OK" << std::endl;

   // ------------------------------------------------------------------------
   // 6. TESTING LIGHTS (Polymorphism)
   // ------------------------------------------------------------------------
   std::cout << "[TEST] Lights... ";

   // 6.1 Infinite Light (Directional)
   glm::vec3 dir(0.0f, -1.0f, 0.0f);
   InfiniteLight* dirLight = new InfiniteLight("Sole", glm::mat4(1.0f), ambient, diffuse, specular, dir);

   // Verifica che w=0 nella posizione (caratteristica luce direzionale)
   assert(dirLight->getPosition().w == 0.0f);
   assert(areVec3Equal(dirLight->getDirection(), glm::normalize(dir)));

   // 6.2 Omnidirectional Light (Point)
   OmnidirectionalLight* omniLight = new OmnidirectionalLight("Lampadina", glm::mat4(1.0f), ambient, diffuse, specular);

   // Verifica che w=1 nella posizione (caratteristica luce posizionale) [implementazione in omnidirectionalLight.cpp]
   // Nota: Il costruttore prende la matrice, estrae la posizione e setta w=1
   assert(omniLight->getPosition().w == 1.0f);
   assert(omniLight->getCutoff() == 180.0f); // Default omni

   // 6.3 Spot Light
   float cutoff = 45.0f;
   float exponent = 10.0f;
   SpotLight* spotLight = new SpotLight("Torcia", glm::mat4(1.0f), ambient, diffuse, specular, dir, cutoff, exponent);

   assert(spotLight->getCutoff() == 45.0f);
   assert(spotLight->getSpotExponent() == 10.0f);
   assert(areVec3Equal(spotLight->getDirection(), glm::normalize(dir)));

   std::cout << "OK" << std::endl;

   // ------------------------------------------------------------------------
   // 7. TESTING LIST
   // ------------------------------------------------------------------------
   std::cout << "[TEST] List (Render Queue)... ";

   List* renderList = new List();

   // Passiamo un nodo alla lista (senza contesto OpenGL attivo, testiamo solo l'inserimento logico)
   // Nota: 'render()' fallirebbe senza contesto grafico, ma 'pass()' popola solo la struttura dati interna.
   renderList->pass(mesh, glm::mat4(1.0f));

   // Purtroppo 'instances' è protected in list.h, quindi non possiamo verificare size() direttamente
   // senza modificare la classe o fare un mock. 
   // Testiamo che 'clear' non crashi.
   renderList->clear();

   std::cout << "OK" << std::endl;

   // ------------------------------------------------------------------------
   // CLEANUP
   // ------------------------------------------------------------------------
   delete node1;
   delete node2;
   delete root; // child era stato rimosso, va cancellato a parte
   delete child;
   delete cam;
   delete mat; // Mesh non possiede il materiale (puntatore raw), quindi cancelliamo noi
   delete mesh;
   delete dirLight;
   delete omniLight;
   delete spotLight;
   delete renderList;

   std::cout << "==========================================" << std::endl;
   std::cout << "      TUTTI I TEST SUPERATI CON SUCCESSO  " << std::endl;
   std::cout << "==========================================" << std::endl;

   return 0;
}