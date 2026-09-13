#include "mesh.h"

namespace eng {

    /**
     * @brief Associa un materiale alla mesh.
     * Definisce come la superficie della mesh reagirà alla luce e quali texture utilizzerà.
     * @param mat Puntatore condiviso all'oggetto Material.
     */
    void ENG_API Mesh::set_material(std::shared_ptr<Material> mat) {
        material = mat;
    }

    /**
     * @brief Restituisce il materiale correntemente associato alla mesh.
     * @return std::shared_ptr<Material> Il materiale, oppure nullptr se non assegnato.
     */
    std::shared_ptr<Material> ENG_API Mesh::get_material() const {
        return material;
    }

    // Aggiunge un nuovo livello LOD alla fine della lista
    /**
     * @brief Aggiunge un nuovo Livello di Dettaglio (LOD) alla mesh.
     * L'indice 0 rappresenta tipicamente il modello a massima risoluzione, mentre i successivi
     * rappresentano versioni progressivamente più leggere (meno vertici e facce).
     * @param v Vettore contenente le coordinate 3D dei vertici.
     * @param f Vettore di tuple contenente gli indici dei vertici che formano le facce (triangoli).
     * @param n Vettore contenente le normali dei vertici (usate per l'illuminazione).
     * @param uv Vettore contenente le coordinate UV per la mappatura delle texture.
     */
    void ENG_API Mesh::add_lod(const std::vector<glm::vec3>& v,
        const std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& f,
        const std::vector<glm::vec3>& n,
        const std::vector<glm::vec2>& uv) {

        LOD new_lod;
        new_lod.vertices = v;
        new_lod.faces = f;
        new_lod.normals = n;
        new_lod.uvs = uv;
        lods.push_back(new_lod);
    }

    /**
     * @brief Restituisce il numero totale di livelli di dettaglio (LOD) caricati per questa mesh.
     * @return uint32_t La quantità di LOD disponibili.
     */
    uint32_t ENG_API Mesh::get_lod_count() const {
        return (uint32_t)lods.size();
    }

    // --- GETTERS (con controllo di sicurezza: se il LOD richiesto non esiste, restituisce l'ultimo disponibile) ---

    /**
     * @brief Restituisce i vertici per il livello di dettaglio specificato.
     * Se il livello richiesto è superiore a quelli disponibili, restituisce i vertici del LOD più basso.
     * @param lod_level L'indice del livello di dettaglio richiesto (default = 0, massima qualità).
     * @return const std::vector<glm::vec3>& Riferimento al vettore dei vertici.
     */
    const std::vector<glm::vec3>& Mesh::get_vertices(uint32_t lod_level) const {
        if (lods.empty()) static std::vector<glm::vec3> empty; if (lod_level >= lods.size()) lod_level = lods.size() - 1;
        return lods[lod_level].vertices;
    }

    /**
     * @brief Restituisce le normali per il livello di dettaglio specificato.
     * Se il livello richiesto è superiore a quelli disponibili, restituisce le normali del LOD più basso.
     * @param lod_level L'indice del livello di dettaglio richiesto.
     * @return const std::vector<glm::vec3>& Riferimento al vettore delle normali.
     */
    const std::vector<glm::vec3>& Mesh::get_normals(uint32_t lod_level) const {
        if (lods.empty()) static std::vector<glm::vec3> empty; if (lod_level >= lods.size()) lod_level = lods.size() - 1;
        return lods[lod_level].normals;
    }

    /**
     * @brief Restituisce le coordinate UV (texture) per il livello di dettaglio specificato.
     * Se il livello richiesto è superiore a quelli disponibili, restituisce le UV del LOD più basso.
     * @param lod_level L'indice del livello di dettaglio richiesto.
     * @return const std::vector<glm::vec2>& Riferimento al vettore delle coordinate UV.
     */
    const std::vector<glm::vec2>& Mesh::get_uvs(uint32_t lod_level) const {
        if (lods.empty()) static std::vector<glm::vec2> empty; if (lod_level >= lods.size()) lod_level = lods.size() - 1;
        return lods[lod_level].uvs;
    }

    /**
     * @brief Restituisce gli indici delle facce (triangoli) per il livello di dettaglio specificato.
     * Se il livello richiesto è superiore a quelli disponibili, restituisce le facce del LOD più basso.
     * @param lod_level L'indice del livello di dettaglio richiesto.
     * @return const std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& Riferimento al vettore delle facce.
     */
    const std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>& Mesh::get_faces(uint32_t lod_level) const {
        if (lods.empty()) static std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> empty; if (lod_level >= lods.size()) lod_level = lods.size() - 1;
        return lods[lod_level].faces;
    }
}