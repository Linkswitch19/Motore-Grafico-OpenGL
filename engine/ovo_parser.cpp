/**
 * @file    ovo_parser.cpp
 * @brief   Implementation of the OVOParser class.
 *
 * Handles the binary parsing of .ovo files to construct the scene graph,
 * including nodes, meshes, materials, lights, and textures.
 */

#include "ovo_parser.h"

#include <iostream>

#include <memory>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#define GLM_ENABLE_EXPERIMENTAL

#ifdef _WINDOWS
#pragma warning(disable:4996) // Disable Visual Studio warning for unsafe fopen
#endif

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "directional_light.h"
#include "material.h"
#include "mesh.h"
#include "node.h"
#include "point_light.h"
#include "spot_light.h"
#include "texture.h"

using namespace eng;

// Mappa dove salvo nome materiale
std::unordered_map<std::string, std::shared_ptr<Material>> OVOParser::materials;

/**
 * @brief Parses an OVO file and constructs the scene graph.
 *
 * Opens the binary file, reads chunk headers, and dispatches parsing to specific
 * functions based on the chunk type (Node, Material, Light, Mesh).
 * It maintains a hierarchy stack to correctly parent nodes during traversal.
 *
 * @param path The file path to the .ovo file.
 * @return std::shared_ptr<Node> The root node of the parsed scene graph.
 */


//mette sempre quello che sto leggendo e il numero di fliglio o nel caso di materiali salvo diretto
std::shared_ptr<Node> ENG_API OVOParser::from_file(const std::string path) {
    OVOParser::materials.clear();

    FILE* file = fopen(path.c_str(), "rb");
    if (file == nullptr) ERROR("Failed to read file '%s'", path.c_str());

    DEBUG("Loading file '%s'...", path.c_str());

    std::stack<std::pair<std::shared_ptr<Node>, uint32_t>> hierarchy;
    std::shared_ptr<Node> root = std::make_shared<Node>();
    root->set_name("Scene Root");

    hierarchy.push(std::make_pair(root, 1));

    while (true) {
        uint32_t type;
        uint32_t size;

        fread(&type, sizeof(uint32_t), 1, file);
        if (feof(file)) break;

        fread(&size, sizeof(uint32_t), 1, file);

        uint8_t* data = new uint8_t[size];
        fread(data, sizeof(uint8_t), size, file);

        if (type == 0) { // Version Chunk
            if (size >= sizeof(uint32_t)) { 
                uint32_t version;
                //sposta  versione=data
                memcpy(&version, data, sizeof(uint32_t));
                DEBUG("OVO version: %d", version);
            }
        }
        else if (type == 1) { // Node Chunk
            //indica anche il numero di figli
            const std::pair<std::shared_ptr<Node>, uint32_t> ret = OVOParser::parse_node_chunk(data, size);
            auto& top = hierarchy.top();
            top.first->add_child(ret.first); // carica figli
            top.second--;//diminuisce contatore
            assert(top.second >= 0);
            hierarchy.push(ret);
        }
        else if (type == 9) { // Material Chunk
            const std::pair<std::shared_ptr<Material>, std::string> ret = OVOParser::parse_material_chunk(data, size);
            const std::shared_ptr<Material> material = ret.first;
            const std::string mat_name = ret.second;
            OVOParser::materials[mat_name] = material;
        }
        else if (type == 16) { // Light Chunk
            const std::pair<std::shared_ptr<Light>, uint32_t> ret = OVOParser::parse_light_chunk(data, size);
            auto& top = hierarchy.top();
            top.first->add_child(ret.first);
            top.second--;
            assert(top.second >= 0);
            hierarchy.push(ret);
        }
        else if (type == 18) { // Mesh Chunk
            const std::pair<std::shared_ptr<Mesh>, uint32_t> ret = OVOParser::parse_mesh_chunk(data, size);
            auto& top = hierarchy.top();
            top.first->add_child(ret.first);
            top.second--;
            assert(top.second >= 0);
            hierarchy.push(ret);
        }
        else {
            WARN("Unsupported OVO chunk type: %d", type);
        }

        while (hierarchy.size() > 0 && hierarchy.top().second == 0) {
            hierarchy.pop();
        }

        delete[] data;
    }

    fclose(file);
    DEBUG("File '%s' loaded successfully.", path.c_str());
    return root;
}

/**
 * @brief Parses a standard Node chunk (Type 1).
 * Reads the name, matrix, and number of children.
 *
 * @param data Pointer to the raw chunk data.
 * @param size Size of the chunk in bytes.
 * @return std::pair<std::shared_ptr<Node>, uint32_t> The parsed Node and its child count.
 */
std::pair<std::shared_ptr<Node>, uint32_t> ENG_API OVOParser::parse_node_chunk(const uint8_t* data, const uint32_t size) {
    std::shared_ptr<Node> node = std::make_shared<Node>();
    uint32_t ptr = 0;

    // Name
    {
        const std::string name = OVOParser::parse_string(data + ptr);
        ptr += name.length() + 1;
        node->set_name(name);
    }

    // Matrix
    {
        glm::mat4 matr;
        memcpy(&matr, data + ptr, sizeof(glm::mat4));
        ptr += sizeof(glm::mat4);
        node->set_base_matrix(matr);
    }

    // Child Count
    uint32_t child_cnt;
    {
        memcpy(&child_cnt, data + ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
    }

    return std::make_pair(node, child_cnt);
}

/**
 * @brief Parses a Mesh chunk (Type 18).
 * Reads geometry data (vertices, normals, UVs, faces), physics hulls, and LODs.
 * Links the mesh to a material if defined.
 *
 * @param data Pointer to the raw chunk data.
 * @param size Size of the chunk in bytes.
 * @return std::pair<std::shared_ptr<Mesh>, uint32_t> The parsed Mesh and its child count.
 */
std::pair<std::shared_ptr<Mesh>, uint32_t> ENG_API OVOParser::parse_mesh_chunk(const uint8_t* data, const uint32_t size) {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
    uint32_t ptr = 0;

    // Name
    {
        const std::string name = OVOParser::parse_string(data + ptr);
        ptr += name.length() + 1;
        mesh->set_name(name);
    }

    // Matrix
    {
        glm::mat4 matr;
        memcpy(&matr, data + ptr, sizeof(glm::mat4));
        ptr += sizeof(glm::mat4);
        mesh->set_base_matrix(matr);
    }

    // Child count
    uint32_t child_cnt;
    {
        memcpy(&child_cnt, data + ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
    }

    ptr += OVOParser::parse_string(data + ptr).length() + 1; // Subtype name?
    ptr += sizeof(uint8_t);

    // Material Link
    {
        const std::string mat_name = OVOParser::parse_string(data + ptr);
        ptr += mat_name.length() + 1;
        if (mat_name == "[none]");
        else if (OVOParser::materials.find(mat_name) == OVOParser::materials.end()) {
            WARN("Material %s not found in material library.", mat_name.c_str());
        }
        else {
            mesh->set_material(OVOParser::materials[mat_name]);
        }
    }

    //dati che non servono
    ptr += sizeof(float); // Radius/Box
    ptr += sizeof(glm::vec3); // Min Box
    ptr += sizeof(glm::vec3); // Max Box

    // Physics Data si ignora la fisica
    {
        uint8_t has_phys;
        memcpy(&has_phys, data + ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        if (has_phys) {
            DEBUG("Physics present at ptr=%u", ptr);
            ptr += 40; // Skip physics properties
            uint32_t hull_cnt;
            memcpy(&hull_cnt, data + ptr, sizeof(uint32_t));
            ptr += sizeof(uint32_t);
            ptr += 20;

            // Skip hull data
            for (uint32_t i = 0; i < hull_cnt; i++) {
                uint32_t hull_vert_cnt;
                memcpy(&hull_vert_cnt, data + ptr, sizeof(uint32_t));
                ptr += sizeof(uint32_t);
                uint32_t hull_faces_cnt;
                memcpy(&hull_faces_cnt, data + ptr, sizeof(uint32_t));
                ptr += sizeof(uint32_t);
                ptr += sizeof(glm::vec3) + hull_vert_cnt * sizeof(glm::vec3) + hull_faces_cnt * 3 * sizeof(uint32_t);
            }
        }
    }

    // LOD Data
    uint32_t lod_cnt;
    memcpy(&lod_cnt, data + ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);


    // Parse ALL LODs
    for (uint32_t i = 0; i < lod_cnt; i++) {
        uint32_t vert_cnt;
        memcpy(&vert_cnt, data + ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        uint32_t face_cnt;
        memcpy(&face_cnt, data + ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;

        // Vertices
        for (uint32_t v = 0; v < vert_cnt; v++) {
            glm::vec3 vertex;
            memcpy(&vertex, data + ptr, sizeof(glm::vec3));
            ptr += sizeof(glm::vec3);
            vertices.push_back(vertex);

            uint32_t normal_raw;
            memcpy(&normal_raw, data + ptr, sizeof(uint32_t));
            ptr += sizeof(uint32_t);
            const glm::vec3 normal = glm::vec3(glm::unpackSnorm3x10_1x2(normal_raw));
            normals.push_back(normal);

            uint32_t uv_raw;
            memcpy(&uv_raw, data + ptr, sizeof(uint32_t));
            ptr += sizeof(uint32_t);
            const glm::vec2 uv = glm::unpackHalf2x16(uv_raw);
            uvs.push_back(uv);

            ptr += sizeof(uint32_t); // Tangent/Padding
        }

        // Faces
        std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> faces;
        for (uint32_t f = 0; f < face_cnt; f++) {
            uint32_t f0, f1, f2;
            memcpy(&f0, data + ptr, sizeof(uint32_t));
            ptr += sizeof(uint32_t);
            memcpy(&f1, data + ptr, sizeof(uint32_t));
            ptr += sizeof(uint32_t);
            memcpy(&f2, data + ptr, sizeof(uint32_t));
            ptr += sizeof(uint32_t);

            const auto face = std::make_tuple(f0, f1, f2);
            faces.push_back(face);
        }

        
        mesh->add_lod(vertices, faces, normals, uvs);
    }

    return std::make_pair(mesh, child_cnt);
}

/**
 * @brief Parses a Material chunk (Type 9).
 * Reads material properties (albedo, roughness, emission) and texture names.
 *
 * @param data Pointer to the raw chunk data.
 * @param size Size of the chunk in bytes.
 * @return std::pair<std::shared_ptr<Material>, std::string> The parsed Material and its unique name.
 */
std::pair<std::shared_ptr<Material>, std::string> ENG_API OVOParser::parse_material_chunk(const uint8_t* data, const uint32_t size) {
    std::shared_ptr<Material> material = std::make_shared<Material>();
    uint32_t ptr = 0;
    std::string mat_name = OVOParser::parse_string(data + ptr);
    // Name
    {
        ;
        ptr += mat_name.length() + 1;
        material->set_name(mat_name);
    }

    // Emission
    {
        glm::vec3 emission;
        memcpy(&emission, data + ptr, sizeof(glm::vec3));
        ptr += sizeof(glm::vec3);
        material->set_emission_color(emission);
        printf("[DEBUG] " "Materiale: %s | Emissione letta: R:%.2f G:%.2f B:%.2f" "\n", mat_name.c_str(), emission.r, emission.g, emission.b);
    }

    // Albedo
    glm::vec3 albedo;
    {
        memcpy(&albedo, data + ptr, sizeof(glm::vec3));
        ptr += sizeof(glm::vec3);
    }

    // Roughness
    float roughness;
    {
        memcpy(&roughness, data + ptr, sizeof(float));
        ptr += sizeof(float);
    }

    ptr += sizeof(float); // Metallic
    ptr += sizeof(float); // Transparency

    // Texture Name
    {
        std::string tex_name = OVOParser::parse_string(data + ptr);
        ptr += tex_name.length() + 1;
        if (tex_name != "[none]") {
            std::shared_ptr<Texture> texture = std::make_shared<Texture>(tex_name);
            material->set_texture(texture);
        }
    }

    // Skip extra maps (Normal, Height, etc.)
    ptr += OVOParser::parse_string(data + ptr).length() + 1;
    ptr += OVOParser::parse_string(data + ptr).length() + 1;
    ptr += OVOParser::parse_string(data + ptr).length() + 1;
    ptr += OVOParser::parse_string(data + ptr).length() + 1;

    // Set standard properties
    material->set_ambient_color(albedo + (material->get_emission_color() * 2.0f));
    material->set_diffuse_color(albedo * 0.5);
    material->set_specular_color(albedo);
   
    material->set_shininess((1.0f - std::sqrt(roughness)) * 128.0f);

    DEBUG("Parsed material '%s'", material->get_name().c_str());
    return std::make_pair(material, material->get_name());
}

/**
 * @brief Parses a Light chunk (Type 16).
 * Handles Point, Directional, and Spot lights based on subtype.
 *
 * @param data Pointer to the raw chunk data.
 * @param size Size of the chunk in bytes.
 * @return std::pair<std::shared_ptr<Light>, uint32_t> The parsed Light node and its child count.
 */
std::pair<std::shared_ptr<Light>, uint32_t> ENG_API OVOParser::parse_light_chunk(const uint8_t* data, const uint32_t size) {
    uint32_t ptr = 0;
    uint32_t child_cnt;
    std::string light_name;

    {
        light_name = OVOParser::parse_string(data + ptr);
        ptr += light_name.length() + 1;
    }

    glm::mat4 matrix;
    {
        memcpy(&matrix, data + ptr, sizeof(glm::mat4));
        ptr += sizeof(glm::mat4);
    }

    {
        memcpy(&child_cnt, data + ptr, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
    }

    ptr += OVOParser::parse_string(data + ptr).length() + 1; // Light target name

    uint8_t subtype;
    {
        memcpy(&subtype, data + ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
    }

    glm::vec3 color;
    {
        memcpy(&color, data + ptr, sizeof(glm::vec3));
        ptr += sizeof(glm::vec3);
    }

    float radius;
    {
        memcpy(&radius, data + ptr, sizeof(float));
        ptr += sizeof(float);
    }

    glm::vec3 direction;
    {
        memcpy(&direction, data + ptr, sizeof(glm::vec3));
        ptr += sizeof(glm::vec3);
    }

    float cutoff;
    {
        memcpy(&cutoff, data + ptr, sizeof(float));
        ptr += sizeof(float);
    }

    float spot_exponent;
    {
        memcpy(&spot_exponent, data + ptr, sizeof(float));
        ptr += sizeof(float);
    }

    if (subtype == 0) { // Point Light
        std::shared_ptr<PointLight> light = std::make_shared<PointLight>();
        light->set_name(light_name);
        light->set_base_matrix(matrix);
        light->set_diffuse_color(color);
        light->set_specular_color(color);
        light->set_radius(radius / 1000.0f);
        return std::make_pair(light, child_cnt);
    }
    else if (subtype == 1) { // Directional Light
        std::shared_ptr<DirectionalLight> light = std::make_shared<DirectionalLight>();
        light->set_name(light_name);
        light->set_base_matrix(matrix);
        light->set_diffuse_color(color);
        light->set_specular_color(color);
        light->set_direction(direction);
        return std::make_pair(light, child_cnt);
    }
    else if (subtype == 2) { // Spot Light
        std::shared_ptr<SpotLight> light = std::make_shared<SpotLight>();
        light->set_name(light_name);
        light->set_base_matrix(matrix);
        light->set_diffuse_color(color);
        light->set_specular_color(color);
        light->set_cutoff(cutoff);
        light->set_radius(radius);
        light->set_exponent(spot_exponent);
        light->set_direction(direction);
        return std::make_pair(light, child_cnt);
    }
    else {
        WARN("Unknown light subtype: %d. Defaulting to a point light.", (uint32_t)subtype);
        return std::make_pair(std::make_shared<PointLight>(), 0);
    }
}

/**
 * @brief Helper function to read a null-terminated string from binary data.
 *
 * @param data Pointer to the start of the string in the data buffer.
 * @return std::string The parsed string.
 */
std::string ENG_API OVOParser::parse_string(const uint8_t* data) {
    std::string str;
    uint32_t ptr = 0;
    while (data[ptr] != 0x00) {
        const char character = (char)data[ptr];
        str += character;
        ptr++;
    }
    return str;
}