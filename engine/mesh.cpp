
#include "mesh.h"
namespace eng {

    void ENG_API Mesh::set_material(std::shared_ptr<Material> mat) {
        material = mat;
    }

    std::shared_ptr<Material> ENG_API Mesh::get_material() const {
        return material;
    }

    void ENG_API Mesh::set_mesh_data(const std::vector<glm::vec3>& v,
        const std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& f,
        const std::vector<glm::vec3>& n,
        const std::vector<glm::vec2>& uv) {
        vertices = v;
        faces = f;
        normals = n;
        uvs = uv;
    }

}
