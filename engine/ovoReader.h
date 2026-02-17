#pragma once

//////////////
// #INCLUDE //
//////////////

    // Header file include

#include <iostream>

#include <map>

// GLM:      
#include <glm/glm.hpp>

#include <glm/gtc/packing.hpp>


#include "node.h"

#include "material.h"

#include "object.h"

#include "mesh.h"

#include "light.h"

#include "spotLight.h"

#include "omnidirectionalLight.h"

#include "infiniteLight.h"

#include "texture.h"







/////////////////
// OVO OBJECTS //
////////////////

/**
 * @class OvObject
 * @brief Represents the base class for various entity types in the engine.
 */
class ENG_API OvObject
{
public:
    enum class Type : int  ///< Type of entities
    {
        // Foundation types:
        OBJECT = 0,
        NODE,
        OBJECT2D,
        OBJECT3D,
        LIST,

        // Derived classes:
        BUFFER,
        SHADER,
        TEXTURE,
        FILTER,
        MATERIAL,
        FBO,
        QUAD,
        BOX,
        SKYBOX,
        FONT,
        CAMERA,
        LIGHT,
        BONE,
        MESH,	   // Keep them...
        SKINNED, // ...consecutive        
        INSTANCED,
        PIPELINE,
        EMITTER,

        // Animation type
        ANIM,

        // Physics related:
        PHYSICS,

        // Terminator:
        LAST,
    };
};

/**
 * @class OvMesh
 * @brief Represents a mesh object and its subtypes.
 */
class ENG_API OvMesh
{
public:
    enum class Subtype : int ///< Kind of mesh
    {
        // Foundation types:
        DEFAULT = 0,
        NORMALMAPPED,
        TESSELLATED,

        // Terminator:
        LAST,
    };
};

/**
 * @class OvLight
 * @brief Represents a light object and its subtypes.
 */
class ENG_API OvLight
{
public:
    enum class Subtype : int ///< Kind of light
    {
        // Foundation types:
        OMNI = 0,
        DIRECTIONAL,
        SPOT,

        // Terminator:
        LAST,
    };
};

/////////////////
// OVO OBJECTS //
////////////////

/**
 * @class OvoReader
 * @brief Handles parsing and loading of OVO files.
 */
class ENG_API OvoReader {

public:
    /**
     * @brief Reads an OVO file and creates a hierarchical node structure.
     * @param file_path Path to the OVO file.
     * @param texture_dir Directory containing textures.
     * @return Pointer to the root Node, or nullptr if an error occurs.
     */
    Node* readFile(const char* file_path, const char* texture_dir);

protected:
    /**
     * @brief A map that stores materials parsed from the OVO file.
     *
     * The key is the material's name (as a string), and the value is a pointer to the corresponding Material object.
     * This map is used to manage and reference materials during the parsing process.
     */
    std::map<std::string, Material*> m_materials;

    /**
     * @brief Recursively loads nodes and their children from the file.
     * @param dat File pointer to the open OVO file.
     * @param path Path to the file.
     * @return Pointer to the parsed Node, or nullptr if an error occurs.
     */
    Node* recursive_load(FILE* dat, const char* path);

    /**
     * @brief Parses a generic object chunk from the file.
     * @param data Pointer to the chunk data.
     * @param position Current read position in the data.
     */
    void parse_object(char* data, unsigned int& position);

    /**
     * @brief Parses a material chunk from the file.
     * @param data Pointer to the chunk data.
     * @param position Current read position in the data.
     * @param texture_dir Directory containing textures.
     * @return Pointer to the parsed Material object.
     */
    Material* parse_material(char* data, unsigned int& position, const char* texture_dir);

    /**
     * @brief Parses a node chunk from the file.
     * @param data Pointer to the chunk data.
     * @param position Current read position in the data.
     * @param n_children Pointer to store the number of child nodes.
     * @return Pointer to the parsed Node object.
     */
    Node* parse_node(char* data, unsigned int& position, unsigned int* n_children);

    /**
     * @brief Parses a mesh chunk from the file.
     * @param data Pointer to the chunk data.
     * @param position Current read position in the data.
     * @param n_children Pointer to store the number of child nodes.
     * @return Pointer to the parsed Mesh object.
     */
    Mesh* parse_mesh(char* data, unsigned int& position, unsigned int* n_children);

    /**
     * @brief Parses a light chunk from the file.
     * @param data Pointer to the chunk data.
     * @param position Current read position in the data.
     * @param n_children Pointer to store the number of child nodes.
     * @return Pointer to the parsed Light object.
     */
    Light* parse_light(char* data, unsigned int& position, unsigned int* n_children);
};