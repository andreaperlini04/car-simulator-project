#include "mesh.h"
#include <GL/freeglut.h>
#include <iostream>
Mesh::Mesh(const std::string& name)
    : Node(name) {
   
}

Mesh::Mesh(const std::string& name, glm::mat4 matrix, unsigned int faces, unsigned int vertices, Material* material)
    : Node(name), matrix(matrix), numFaces(faces), numVertices(vertices), material(material) {
   this->setM(matrix);

}

const std::vector<glm::vec3>& Mesh::get_all_vertices() const { return all_vertices; }
const std::vector<glm::vec3>& Mesh::get_all_normals() const { return all_normals; }
const std::vector<glm::vec2>& Mesh::get_all_texture_coords() const { return all_texture_coords; }
const std::vector<std::vector<unsigned int>>& Mesh::get_face_vertices() const { return face_vertices; }
Material* Mesh::getMaterial() const { return material; }

void Mesh::set_all_vertices(const std::vector<glm::vec3>& vertices) { all_vertices = vertices; }
void Mesh::set_all_normals(const std::vector<glm::vec3>& normals) { all_normals = normals; }
void Mesh::set_all_texture_coords(const std::vector<glm::vec2>& textureCoords) { all_texture_coords = textureCoords; }
void Mesh::set_face_vertices(const std::vector<std::vector<unsigned int>>& faces) { face_vertices = faces; }
void Mesh::setMaterial(Material* material) { this->material = material; }

void Mesh::render() {
    // 1. Applica Materiale
    if (material) {
        material->render(); // Attiva luci e setta i coefficienti kA, kD, kS
    }
    else {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    // 2. Disegna Geometria
    if (!all_vertices.empty() && !face_vertices.empty()) {
        glBegin(GL_TRIANGLES);

        for (const auto& face : face_vertices) {
            for (unsigned int idx : face) {
                if (idx < all_vertices.size()) {
                    // Applica normale se disponibile
                    if (idx < all_normals.size()) {
                        glNormal3f(all_normals[idx].x, all_normals[idx].y, all_normals[idx].z);
                    }

                    // Applica coordinate texture se disponibili
                    if (idx < all_texture_coords.size()) {
                        glTexCoord2f(all_texture_coords[idx].x, all_texture_coords[idx].y);
                    }

                    // Disegna vertice
                    glVertex3f(all_vertices[idx].x, all_vertices[idx].y, all_vertices[idx].z);
                }
            }
        }

        glEnd();
    }

    // 3. Ripristina stato
    if (!material) glEnable(GL_LIGHTING);
}