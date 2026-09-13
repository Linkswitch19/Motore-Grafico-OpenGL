/**
 * @file	ovo_parser.h
 * @brief	OVO file format parser class definition
 *
 * This file contains the definition of the OVOParser class, which is responsible for parsing
 * OVO files and constructing the corresponding scene graph. The parser reads various chunks
 * from the OVO file, including nodes, meshes, materials, and lights, and builds the scene hierarchy.
 *
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "light.h"
#include "material.h"
#include "mesh.h"
#include "node.h"
#include "lib.h"

namespace eng {

/**
 * @brief OVO file parser class.
 *
 * The OVOParser class is responsible for parsing OVO files and constructing the corresponding scene graph.
 * The parser reads various chunks from the OVO file, including nodes, meshes, materials,
 * and lights, and builds the scene hierarchy. 
 * The class provides a static method to load an OVO file from disk and return the root node of the scene graph.
 *
 * @note The OVO file format is a custom binary format used for 3D scene representation.
 */
class ENG_API OVOParser {
public:
    static std::shared_ptr<Node>                             from_file(const std::string path);
private:
    static std::pair<std::shared_ptr<Node>, uint32_t>        parse_node_chunk(const uint8_t* data, const uint32_t size);
    static std::pair<std::shared_ptr<Mesh>, uint32_t>        parse_mesh_chunk(const uint8_t* data, const uint32_t size);
    static std::pair<std::shared_ptr<Material>, std::string> parse_material_chunk(const uint8_t* data, const uint32_t size);
    static std::pair<std::shared_ptr<Light>, uint32_t>       parse_light_chunk(const uint8_t* data, const uint32_t size);
    static std::string                                       parse_string(const uint8_t* data);
    static std::unordered_map<std::string, std::shared_ptr<Material>> materials;  /**< Material library */
};

}
