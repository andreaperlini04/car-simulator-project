/**
 * @file engine_test.cpp
 * @brief Test suite per i moduli core del motore grafico.
 */

#include <assert.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "object.h"
#include "node.h"
#include "camera.h"
#include "perspectiveCamera.h"
#include "orthographicCamera.h"
#include "mesh.h"
#include "material.h"
#include "light.h"
#include "omnidirectionalLight.h"
#include "infiniteLight.h"
#include "spotLight.h"
#include "list.h"
#include "ovoReader.h"

 // Tolleranza per l'approssimazione floating-point vettoriale/matriciale
#define EPSILON 0.0001f

bool areVec3Equal(const glm::vec3& a, const glm::vec3& b) {
   return glm::length(a - b) < EPSILON;
}

#ifndef ALL_TESTS_UNIFIED
int main() {
#else
void runEngineTests() {
#endif
   std::cout << "==========================================\n";
   std::cout << "      AVVIO ENGINE TEST SUITE (MAIN)      \n";
   std::cout << "==========================================\n";

   // --- Object & Node Base ---
   std::cout << "[TEST] Object & Node Base... ";
   Node* node1 = new Node("NodoTest1");

   assert(node1->getName() == "NodoTest1");

   // Verifica che l'assegnazione degli ID sia atomica e strettamente incrementale
   unsigned int id1 = node1->getId();
   Node* node2 = new Node("NodoTest2");
   unsigned int id2 = node2->getId();
   assert(id1 != id2);
   assert(id2 > id1);

   assert(node1->getM() == glm::mat4(1.0f));

   std::cout << "OK\n";

   // --- Hierarchy & Transformations ---
   std::cout << "[TEST] Node Transformations & Hierarchy... ";
   Node* root = new Node("Root");
   Node* child = new Node("Child");

   root->addChild(child);
   assert(root->getNumChildren() == 1);
   assert(root->getChild(0) == child);
   assert(child->getParent() == root);

   root->translate(glm::vec3(10.0f, 0.0f, 0.0f));
   child->translate(glm::vec3(0.0f, 5.0f, 0.0f));
   glm::mat4 worldMat = child->getWorldFinalMatrix();

   assert(areVec3Equal(glm::vec3(worldMat[3]), glm::vec3(10.0f, 5.0f, 0.0f)));

   root->removeChild(child);
   assert(root->getNumChildren() == 0);
   assert(child->getParent() == nullptr);

   std::cout << "OK\n";

   // --- Cameras ---
   std::cout << "[TEST] Camera... ";
   Camera* cam = new Camera("MainCam");

   glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.33f, 0.1f, 100.0f);
   cam->setProjectionMatrix(proj);
   assert(cam->getProjectionMatrix() == proj);

   // La view matrix deve corrispondere all'inversa della trasformazione spaziale (World Matrix)
   cam->translate(glm::vec3(0.0f, 0.0f, 10.0f));
   glm::mat4 viewMat = cam->getInvCameraMatrix();
   assert(areVec3Equal(glm::vec3(viewMat[3]), glm::vec3(0.0f, 0.0f, -10.0f)));

   OrthographicCamera* orthoCam = new OrthographicCamera("UI_Cam", 0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
   glm::mat4 expectedProj = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
   assert(orthoCam->getProjectionMatrix() == expectedProj);

   std::cout << "OK\n";

   // --- Materials ---
   std::cout << "[TEST] Material... ";
   glm::vec3 ambient(0.2f);
   glm::vec3 diffuse(0.8f, 0.0f, 0.0f);
   glm::vec3 specular(1.0f);
   glm::vec3 emission(0.0f);

   std::shared_ptr<Material> mat = std::make_shared<Material>("RossoSemiTrasp", emission, ambient, diffuse, specular, 32.0f, 0.5f);

   assert(areVec3Equal(mat->getDiffuse(), diffuse));
   assert(mat->getTransparency() == 0.5f);
   assert(mat->getShininess() == 32.0f);

   mat->setTransparency(1.0f);
   assert(mat->getTransparency() == 1.0f);

   std::cout << "OK\n";

   // --- Meshes ---
   std::cout << "[TEST] Mesh... ";
   Mesh* mesh = new Mesh("Triangolo");
   mesh->setMaterial(mat);

   assert(mesh->getMaterial() == mat);
   assert(mesh->getName() == "Triangolo");

   std::vector<glm::vec3> verts = { {0,0,0}, {1,0,0}, {0,1,0} };
   std::vector<glm::vec3> norms = { {0,0,1}, {0,0,1}, {0,0,1} };

   mesh->set_all_vertices(verts);
   mesh->set_all_normals(norms);

   assert(mesh->get_all_vertices().size() == 3);
   assert(mesh->get_all_normals().size() == 3);
   assert(areVec3Equal(mesh->get_all_vertices()[1], glm::vec3(1, 0, 0)));

   std::cout << "OK\n";

   // --- Lights (Polymorphism) ---
   std::cout << "[TEST] Lights... ";

   glm::vec3 dir(0.0f, -1.0f, 0.0f);
   InfiniteLight* dirLight = new InfiniteLight("Sole", glm::mat4(1.0f), ambient, diffuse, specular, dir);
   assert(dirLight->getPosition().w == 0.0f); // W=0 indica un vettore direzionale all'infinito
   assert(areVec3Equal(dirLight->getDirection(), glm::normalize(dir)));

   OmnidirectionalLight* omniLight = new OmnidirectionalLight("Lampadina", glm::mat4(1.0f), ambient, diffuse, specular);
   assert(omniLight->getPosition().w == 1.0f); // W=1 indica una posizione spaziale puntuale
   assert(omniLight->getCutoff() == 180.0f);

   SpotLight* spotLight = new SpotLight("Torcia", glm::mat4(1.0f), ambient, diffuse, specular, dir, 45.0f, 10.0f);
   assert(spotLight->getCutoff() == 45.0f);
   assert(spotLight->getSpotExponent() == 10.0f);
   assert(areVec3Equal(spotLight->getDirection(), glm::normalize(dir)));

   std::cout << "OK\n";

   // --- Render Queue (List) ---
   std::cout << "[TEST] List (Render Queue)... ";
   List* renderList = new List();
   renderList->pass(mesh, glm::mat4(1.0f));
   renderList->clear();
   std::cout << "OK\n";

   // --- Memory & Ownership ---
   std::cout << "[TEST] Memory Management (Ownership Transfer)... ";
   Node* memRoot = new Node("MemRoot");
   Node* memChild = new Node("MemChild");

   memRoot->addChild(memChild);
   assert(memRoot->getNumChildren() == 1);

   // L'estrazione deve rilasciare la ownership del child pointer per evitare double-free
   Node* extracted = memRoot->removeChild(memChild);
   assert(memRoot->getNumChildren() == 0);
   assert(extracted == memChild);
   assert(extracted->getParent() == nullptr);

   delete memRoot;
   delete extracted; // Deallocazione manuale necessaria post-estrazione dalla gerarchia

   std::cout << "OK\n";

   // --- OvoReader ---
   std::cout << "[TEST] OvoReader (Robustness)... ";
   OvoReader reader;

   // Validazione fail-safe: l'assenza di un asset non deve causare crash fatali
   std::cout << "\n   [Atteso un messaggio di errore dal reader qui sotto] \n";
   Node* missingScene = reader.readFile("file_che_non_esiste_assolutamente.ovo", "texture/");

   assert(missingScene == nullptr);
   std::cout << "OK\n";

   // --- Cleanup ---
   delete node1;
   delete node2;
   delete root;
   delete child; // Extracted precedentemente da 'root', ha perso il lifecycle automatico
   delete cam;
   delete orthoCam;
   delete mesh;
   delete dirLight;
   delete omniLight;
   delete spotLight;
   delete renderList;

   std::cout << "==========================================\n";
   std::cout << "  TUTTI I TEST ENGINE SUPERATI CON SUCCESSO\n";
   std::cout << "==========================================\n";

#ifndef ALL_TESTS_UNIFIED
   return 0;
#endif
}