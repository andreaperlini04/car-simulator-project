//////////////
// #INCLUDE //
//////////////
#define _CRT_SECURE_NO_WARNINGS
// Camera.h include
#include "ovoReader.h"
using namespace std;

//GLM

// Macro for printing an OvMatrix4 to console:   
#define MAT2STR(m) cout << "   Matrix  . . . :  \t" << m[0][0] << "\t" << m[1][0] << "\t" << m[2][0] << "\t" << m[3][0] << std::endl \
                           << "                    \t" << m[0][1] << "\t" << m[1][1] << "\t" << m[2][1] << "\t" << m[3][1] << std::endl \
                           << "                    \t" << m[0][2] << "\t" << m[1][2] << "\t" << m[2][2] << "\t" << m[3][2] << std::endl \
                           << "                    \t" << m[0][3] << "\t" << m[1][3] << "\t" << m[2][3] << "\t" << m[3][3] << std::endl 



/////////////
// CLASSES //
/////////////

Node ENG_API* OvoReader::readFile(const char* file_path, const char* texture_dir) {
    FILE* dat = fopen(file_path, "rb");

    //Open File:
    if (dat == nullptr) {
        std::cout << "1-ERROR: unable to open file '" << file_path << "'" << std::endl;
        return nullptr;
    }

    //////////////////////////
    ///   PARSE CHUNKS    ///
   /////////////////////////

   //First we parse the material chunks
    unsigned int chunkId;
    unsigned int chunkSize;

    bool isHeader = true;
    while (isHeader) {
        fread(&chunkId, sizeof(unsigned int), 1, dat);
        if (feof(dat))
            return nullptr;
        fread(&chunkSize, sizeof(unsigned int), 1, dat);


        //Load whole chunk into memory
        char* data = new char[chunkSize];

        if (fread(data, sizeof(char), chunkSize, dat) != chunkSize)
        {
            std::cout << "2-ERROR: unable to read from file '" << file_path << "'" << std::endl;
            fclose(dat);
            delete[] data;
            return nullptr;
        }

        //Parse chunk informations according to its type
        unsigned int position = 0;
        int rollback = 0;
        Material* material;

        switch ((OvObject::Type)chunkId) {

        case OvObject::Type::OBJECT:
            parse_object(data, position);
            break;

        case OvObject::Type::MATERIAL:
            material = parse_material(data, position, texture_dir);
            m_materials.insert(make_pair(material->getName(), material));
            break;

        case OvObject::Type::NODE:
        case OvObject::Type::MESH:
        case OvObject::Type::LIGHT:
        case OvObject::Type::BONE:
        case OvObject::Type::SKINNED:

            //We have done with he header part now we can start marsing other OvObject
            // However, if we not move back the file pointer, we will miss a chunk
            isHeader = false;
            rollback = -1 * static_cast<int>(chunkSize + 8);

            //move the file pointer back from the current position
            fseek(dat, rollback, SEEK_CUR);
            break;

        default:
            std::cout << "3-ERROR: corrupted or bad data in file " << file_path << std::endl;
            fclose(dat);
            delete[] data;
            return nullptr;

        }

        //Release chunk memory
        delete[] data;

    }

    Node* root = recursive_load(dat, file_path);
    fclose(dat);
    std::cout << "\nFile OVO parsed" << std::endl;

    return root;

}

Node ENG_API* OvoReader::recursive_load(FILE* dat, const char* path)
{
    unsigned int chunkId;
    unsigned int chunkSize;
    fread(&chunkId, sizeof(unsigned int), 1, dat);
    if (feof(dat))
        return nullptr;
    fread(&chunkSize, sizeof(unsigned int), 1, dat);

    char* data = new char[chunkSize];

    if (fread(data, sizeof(char), chunkSize, dat) != chunkSize)
    {
        std::cout << "4-ERROR: unable to read from file '" << path << "'" << std::endl;
        fclose(dat);
        delete[] data;
        return nullptr;
    }
    unsigned int position = 0;
    unsigned int n_children = 0;
    Node* this_node = nullptr;
    switch ((OvObject::Type)chunkId) {
    case OvObject::Type::NODE:
        this_node = parse_node(data, position, &n_children);
        break;

    case OvObject::Type::MESH:
        this_node = parse_mesh(data, position, &n_children);
        break;

    case OvObject::Type::LIGHT:
        this_node = parse_light(data, position, &n_children);
        break;

    case OvObject::Type::BONE:
    case OvObject::Type::SKINNED:
        //skipped
        break;

    default:
        std::cout << "5-ERROR: corrupted or bad data in file " << path << std::endl;
        fclose(dat);
        delete[] data;
        return nullptr;

    }

    delete[] data;

    //Recursively parse its children based on n_children value
    for (unsigned int current_children = 0; current_children < n_children; current_children++)
    {
        Node* child_node = recursive_load(dat, path);

        if (child_node != nullptr)
            this_node->addChild(child_node);
    }

    return this_node;


}

void ENG_API OvoReader::parse_object(char* data, unsigned int& position)
{
    unsigned int versionId;
    memcpy(&versionId, data + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
}

Material ENG_API* OvoReader::parse_material(char* data, unsigned int& position, const char* texture_dir)
{
   char materialName[FILENAME_MAX];
   strcpy(materialName, data + position);
   position += strlen(materialName) + 1;


   glm::vec3 emission, albedo;
   float roughness, metalness, transparency;
   

   memcpy(&emission, data + position, sizeof(glm::vec3));
   position += sizeof(glm::vec3);
   memcpy(&albedo, data + position, sizeof(glm::vec3));
   position += sizeof(glm::vec3);
   memcpy(&roughness, data + position, sizeof(float));
   position += sizeof(float);
   memcpy(&metalness, data + position, sizeof(float));
   position += sizeof(float);
   memcpy(&transparency, data + position, sizeof(float));
   position += sizeof(float);

   std::cout << "[OvoReader] Parsing Material: '" << materialName << "' " << transparency<< std::endl; // <--- LOG

   // Texture filenames
   char albedoTexture[FILENAME_MAX];
   strcpy(albedoTexture, data + position);
   position += (unsigned int)strlen(albedoTexture) + 1;

   char normalMapTexture[FILENAME_MAX];
   strcpy(normalMapTexture, data + position);
   position += (unsigned int)strlen(normalMapTexture) + 1;

   char heightMapTexture[FILENAME_MAX];
   strcpy(heightMapTexture, data + position);
   position += (unsigned int)strlen(heightMapTexture) + 1;

   char roughnessTexture[FILENAME_MAX];
   strcpy(roughnessTexture, data + position);
   position += (unsigned int)strlen(roughnessTexture) + 1;

   char metalnessTexture[FILENAME_MAX];
   strcpy(metalnessTexture, data + position);
   position += (unsigned int)strlen(metalnessTexture) + 1;

   // Crea Materiale
   float shininess = pow(1.0f - roughness, 4) * 128.0f;
   glm::vec4 specular4 = glm::vec4(glm::mix(glm::vec3(0.04f), albedo, metalness), 0.0f);
   glm::vec4 emission4 = glm::vec4(emission, 0.0f);
   glm::vec4 albedo4 = glm::vec4(albedo, 0.0f);

   Material* material = new Material(materialName, emission4, albedo4 * 0.1f, albedo4, specular4, shininess, transparency);

   // Gestione Texture (con fix percorso)
   if (std::string{ albedoTexture } != "[none]") {
      std::string path = texture_dir;
      // Fix slash
      if (!path.empty() && path.back() != '/' && path.back() != '\\') {
         path += "/";
      }
      path += albedoTexture;

      std::cout << "   [Texture] Loading Albedo: " << path << std::endl; // <--- LOG

      Texture* t = new Texture{ albedoTexture, path };
      material->setTexture(t);
   }

   return material;
}

Node ENG_API* OvoReader::parse_node(char* data, unsigned int& position, unsigned int* n_children)
{

   char nodeName[FILENAME_MAX];
   strncpy(nodeName, data + position, FILENAME_MAX - 1);
   nodeName[FILENAME_MAX - 1] = '\0';
   position += (unsigned int)strlen(nodeName) + 1;

   std::cout << "[OvoReader] Node found: '" << nodeName << "'" << std::endl; // <--- LOG

   glm::mat4 matrix;
   memcpy(&matrix, data + position, sizeof(glm::mat4));
   position += sizeof(glm::mat4);

   unsigned int children;
   memcpy(&children, data + position, sizeof(unsigned int));
   *n_children = children;
   position += sizeof(unsigned int);

   char targetName[FILENAME_MAX];
   strcpy(targetName, data + position);
   position += (unsigned int)strlen(targetName) + 1;

   Node* node = new Node{ nodeName };
   node->setM(matrix);
   return node;
}

Mesh ENG_API* OvoReader::parse_mesh(char* data, unsigned int& position, unsigned int* n_children)
{
    // Mesh name (optional for reference, not stored)
    char meshName[FILENAME_MAX];
    strcpy(meshName, data + position);
    position += (unsigned int)strlen(meshName) + 1;
    // Mesh matrix
    glm::mat4 matrix;
    memcpy(&matrix, data + position, sizeof(glm::mat4));
  
    position += sizeof(glm::mat4);

    // Number of children nodes
    unsigned int children;
    memcpy(&children, data + position, sizeof(unsigned int));
    *n_children = children;
    position += sizeof(unsigned int);

    // Optional target node, or [none] if not used:
    char targetName[FILENAME_MAX];
    strcpy(targetName, data + position);
    position += (unsigned int)strlen(targetName) + 1;

    // Mesh subtype
    unsigned char subtype;
    memcpy(&subtype, data + position, sizeof(unsigned char));
    char subtypeName[FILENAME_MAX];
    switch ((OvMesh::Subtype)subtype)
    {
    case OvMesh::Subtype::DEFAULT: strcpy(subtypeName, "standard"); break;
    case OvMesh::Subtype::NORMALMAPPED: strcpy(subtypeName, "normal-mapped"); break;
    case OvMesh::Subtype::TESSELLATED: strcpy(subtypeName, "tessellated"); break;
    default: strcpy(subtypeName, "UNDEFINED");
    }
    position += sizeof(unsigned char);


    // Material name
    char materialName[FILENAME_MAX];
    strcpy(materialName, data + position);
    position += (unsigned int)strlen(materialName) + 1;

    // --- LOG ---
    std::cout << "[OvoReader] Mesh found: '" << meshName
       << "' -> Material: '" << materialName << "'" << std::endl;
    // -----------

    // Mesh bounding sphere radius:
    float radius;
    memcpy(&radius, data + position, sizeof(float));
    position += sizeof(float);

    // Mesh bounding box minimum corner:
    glm::vec3 bBoxMin;
    memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
    position += sizeof(glm::vec3);

    // Mesh bounding box maximum corner:
    glm::vec3 bBoxMax;
    memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
    position += sizeof(glm::vec3);

    // Optional physics properties; thery are not used in our engine but necessary to move the file pointer
    unsigned char hasPhysics;
    memcpy(&hasPhysics, data + position, sizeof(unsigned char));
    position += sizeof(unsigned char);

    if (hasPhysics) {
        struct PhysProps
        {
            // Pay attention to 16 byte alignement (use padding):      
            unsigned char type;
            unsigned char contCollisionDetection;
            unsigned char collideWithRBodies;
            unsigned char hullType;

            // Vector data:
            glm::vec3 massCenter;

            // Mesh properties:
            float mass;
            float staticFriction;
            float dynamicFriction;
            float bounciness;
            float linearDamping;
            float angularDamping;
            unsigned int nrOfHulls;
            unsigned int _pad;

            // Pointers:
            void* physObj;
            void* hull;
        };
        PhysProps mp;
        memcpy(&mp, data + position, sizeof(PhysProps));
        position += sizeof(PhysProps);


        if (mp.nrOfHulls) {
            for (unsigned int c = 0; c < mp.nrOfHulls; c++)
            {
                // Hull number of vertices:
                unsigned int nrOfVertices;
                memcpy(&nrOfVertices, data + position, sizeof(unsigned int));
                position += sizeof(unsigned int);
                // Hull number of faces:
                unsigned int nrOfFaces;
                memcpy(&nrOfFaces, data + position, sizeof(unsigned int));
                position += sizeof(unsigned int);
                // Hull centroid:
                position += sizeof(glm::vec3);
                // Iterate through hull vertices:
                for (unsigned int i = 0; i < nrOfVertices; i++)
                {
                    // Vertex coords:
                    position += sizeof(glm::vec3);
                }
                // Iterate through hull faces:
                for (unsigned int i = 0; i < nrOfFaces; i++)
                {
                    position += sizeof(unsigned int) * 3;
                }
            }
        }
    }

    // For each LOD (assuming only one LOD is relevant here)
    unsigned int LODs;
    memcpy(&LODs, data + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
    //!!! Warning: always loading the first level of deteails (always available)

    unsigned int vertices;
    unsigned int faces;

    memcpy(&vertices, data + position, sizeof(unsigned int));
    position += sizeof(unsigned int);

    // Number of faces
    memcpy(&faces, data + position, sizeof(unsigned int));
    position += sizeof(unsigned int);

    std::vector<glm::vec3> vertexData;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> facesCoords;

    for (unsigned int c = 0; c < vertices; c++)
    {


        //Vertex coord:
        glm::vec3 vertex;
        memcpy(&vertex, data + position, sizeof(glm::vec3));
        position += sizeof(glm::vec3);


        // Vertex normal:
        unsigned int normalData;
        memcpy(&normalData, data + position, sizeof(unsigned int));
        glm::vec4 normal = glm::unpackSnorm3x10_1x2(normalData);
        position += sizeof(unsigned int);


        // Texture coordinates:
        unsigned int textureData;
        memcpy(&textureData, data + position, sizeof(unsigned int));
        glm::vec2 uv = glm::unpackHalf2x16(textureData);
        position += sizeof(unsigned int);


        // Tangent vector:
        position += sizeof(unsigned int);

        vertexData.push_back(vertex);
        normals.push_back(normal);
        textureCoords.push_back(uv);
    }


    //Every face is composed by three vertices
    std::vector<std::vector<unsigned int>> facesData;

    for (unsigned int c = 0; c < faces; c++)
    {
        unsigned int face[3];
        memcpy(face, data + position, sizeof(unsigned int) * 3);
        position += sizeof(unsigned int) * 3;
        std::vector<unsigned int> faceArray = { face[0], face[1], face[2] };
        facesData.push_back(faceArray);

    }


    auto material = m_materials.find(materialName);
    if (material == m_materials.end()) {
        std::cout << "ERROR: material '" << materialName << "' doesn't exists in file" << std::endl;
        return nullptr;
    }

    Mesh* mesh = new Mesh{ meshName, matrix, faces, vertices, material->second };
    mesh->set_all_vertices(vertexData);
    mesh->set_all_normals(normals);
    mesh->set_all_texture_coords(textureCoords);
    mesh->set_face_vertices(facesData);

    std::cout << "   -> Vertices: " << vertices << ", Faces: " << faces << std::endl; // <--- LOG

    return mesh;
}

Light ENG_API* OvoReader::parse_light(char* data, unsigned int& position, unsigned int* n_children)
{
   
    // Nome della luce
    char lightName[FILENAME_MAX];
    strcpy(lightName, data + position);
    position += (unsigned int)strlen(lightName) + 1;

    glm::mat4 matrix;
    memcpy(&matrix, data + position, sizeof(glm::mat4));
    position += sizeof(glm::mat4);

    // Numero di figli (non usato direttamente)
    unsigned int children;
    memcpy(&children, data + position, sizeof(unsigned int));
    *n_children = children;
    position += sizeof(unsigned int);

    // Target node (non usato direttamente in questo caso)
    char targetName[FILENAME_MAX];
    strcpy(targetName, data + position);
    position += (unsigned int)strlen(targetName) + 1;

    // Subtipo della luce (0 = omni, 1 = directional, 2 = spot)
    unsigned char subtype;
    memcpy(&subtype, data + position, sizeof(unsigned char));
    char subtypeName[FILENAME_MAX];
    switch ((OvLight::Subtype)subtype)
    {
    case OvLight::Subtype::DIRECTIONAL: strcpy(subtypeName, "directional"); break;
    case OvLight::Subtype::OMNI: strcpy(subtypeName, "omni"); break;
    case OvLight::Subtype::SPOT: strcpy(subtypeName, "spot"); break;
    default: strcpy(subtypeName, "UNDEFINED");
    }
    position += sizeof(unsigned char);

    std::cout << "[OvoReader] Light found: '" << lightName << "' (" << subtypeName << ")" << std::endl; // <--- LOG

    // Colore (ambient, diffuse, specular)
    glm::vec3 color;
    memcpy(&color, data + position, sizeof(glm::vec3));
    position += sizeof(glm::vec3);

    // Raggio (solo per luci Omni e Spot)
    float radius;
    memcpy(&radius, data + position, sizeof(float));
    position += sizeof(float);

    // Direzione (solo per luci Spot)
    glm::vec3 direction;
    memcpy(&direction, data + position, sizeof(glm::vec3));
    //std::cout << direction.x << " "<< direction.y << " "<< direction.z << " "<< std::endl;
    position += sizeof(glm::vec3);

    // Angolo di cutoff (solo per luci Spot)
    float cutoff;
    memcpy(&cutoff, data + position, sizeof(float));
    position += sizeof(float);

    // Exponent:
    float spotExponent;
    memcpy(&spotExponent, data + position, sizeof(float));
    position += sizeof(float);

    // Shadow cast (1 = sì, 0 = no)
    unsigned char castShadows;
    memcpy(&castShadows, data + position, sizeof(unsigned char));
    position += sizeof(unsigned char);

    // Volumetrica (1 = sì, 0 = no)
    unsigned char isVolumetric;
    memcpy(&isVolumetric, data + position, sizeof(unsigned char));
    position += sizeof(unsigned char);

    // Crea l'oggetto di tipo appropriato in base al tipo di luce
    Light* light = nullptr;
    float attenuation;

    if ((OvLight::Subtype)subtype == OvLight::Subtype::OMNI) { // OmniDirectionalLight
        attenuation = glm::clamp(1.0f / radius, 0.0f, 1.0f);
        OmnidirectionalLight* omniLight = new OmnidirectionalLight(
            lightName, matrix, color, color, color
            //,constantAttenuation, linearAttenuation, quadraticAttenuation
        );
        light = omniLight;
    }
    else if ((OvLight::Subtype)subtype == OvLight::Subtype::DIRECTIONAL) { // DirectionalLight
        InfiniteLight* infiniteLight = new InfiniteLight(
            lightName, matrix, color, color, color, direction
        );  // Le luci direzionali non hanno attenuazione, quindi non servono questi parametri
        light = infiniteLight;
    }
    else if ((OvLight::Subtype)subtype == OvLight::Subtype::SPOT) { // SpotLight
        attenuation = glm::clamp(10.0f / radius, 0.0f, 1.0f);
        SpotLight* spotLight = new SpotLight(
            lightName, matrix, color, color, color, direction, cutoff, spotExponent
            //, constantAttenuation,linearAttenuation, quadraticAttenuation
        );
        light = spotLight;
    }

    return light;
}