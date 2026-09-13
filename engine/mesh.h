#pragma once
#include <vector>
#include <tuple>
#include <memory>
#include "node.h"
#include "material.h"

namespace eng {

    // Nuova struttura che raggruppa i dati di un singolo Livello di Dettaglio
    struct LOD {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> faces;
    };

    class ENG_API Mesh : public Node {
    private:
        std::shared_ptr<Material> material;
        std::vector<LOD> lods; // Lista di tutti i LOD disponibili

    public:
        Mesh() = default;
        virtual ~Mesh() = default;

        void set_material(std::shared_ptr<Material> mat);
        std::shared_ptr<Material> get_material() const;

        // Cambiato da set_mesh_data ad add_lod
        void add_lod(const std::vector<glm::vec3>& v,
            const std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& f,
            const std::vector<glm::vec3>& n,
            const std::vector<glm::vec2>& uv);

        uint32_t get_lod_count() const;

        // I getter ora accettano l'indice del LOD (di base 0 = qualità massima)
        const std::vector<glm::vec3>& get_vertices(uint32_t lod_level = 0) const;
        const std::vector<glm::vec3>& get_normals(uint32_t lod_level = 0) const;
        const std::vector<glm::vec2>& get_uvs(uint32_t lod_level = 0) const;
        const std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& get_faces(uint32_t lod_level = 0) const;
    };
}