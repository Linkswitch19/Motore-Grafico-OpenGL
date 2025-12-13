#pragma once
#include <vector>
#include <tuple>
#include <memory>
#include "node.h"
#include "material.h"

namespace eng {

    class ENG_API Mesh : public Node {
    private:
        std::shared_ptr<Material> material;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> faces;

    public:
        Mesh() = default;
        virtual ~Mesh() = default;

        void set_material(std::shared_ptr<Material> mat);
        std::shared_ptr<Material> get_material() const;

        void set_mesh_data(const std::vector<glm::vec3>& v,
            const std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& f,
            const std::vector<glm::vec3>& n,
            const std::vector<glm::vec2>& uv);

        const std::vector<glm::vec3>& get_vertices() const;
        const std::vector<glm::vec3>& get_normals() const;
        const std::vector<glm::vec2>& get_uvs() const;
        const std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& get_faces() const;
    };

       

}