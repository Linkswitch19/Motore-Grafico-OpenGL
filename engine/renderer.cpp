#include "renderer.h"
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
/**
 * @file renderer.cpp
 * @brief Implementazione del sistema di rendering della scena.
 *
 * Contiene le funzioni per il rendering della scena OVO, gestione luci,
 * materiali, mesh e ombre tramite OpenGL fixed pipeline.
 */
namespace eng {
    /**
    * @brief Numero corrente di luci attive nella scena.
    *
    * Utilizzato per assegnare progressivamente gli ID OpenGL
    * (GL_LIGHT1 ... GL_LIGHT7).
    */

    int currentLightCount = 0;

    /**
     * @brief Renderizza l'intera scena a partire dal nodo radice.
     *
     * Applica una trasformazione iniziale della scena e avvia
     * la visita ricorsiva dei nodi.
     *
     * @param root Nodo radice della scena
     * @param viewMatrix Matrice di vista della camera
     * @param currentLOD Livello di dettaglio da utilizzare per le mesh
     */
    void Renderer::renderScene(std::shared_ptr<Node> root, const glm::mat4& viewMatrix, int currentLOD) {
        if (!root) return;
        currentLightCount = 0;
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, -40.0f));
        glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

        renderOvoNode(root, modelViewMatrix, currentLOD); // Passalo qui!
    }
    /**
   * @brief Visita ricorsivamente un nodo della scena e lo renderizza.
   *
   * Gestisce:
   * - Luci
   * - Mesh
   * - Trasformazioni gerarchiche
   *
   * @param node Nodo corrente
   * @param parentMatrix Matrice globale del nodo padre
   * @param currentLOD Livello di dettaglio delle mesh
   */
    void Renderer::renderOvoNode(std::shared_ptr<Node> node, glm::mat4 parentMatrix, int currentLOD) {
        if (!node) return;

        glm::mat4 globalMatrix = parentMatrix * node->get_base_matrix();

        // 1. Gestione Luci
        if (auto light = std::dynamic_pointer_cast<eng::Light>(node)) {
            processLight(light, globalMatrix);
        }

        // 2. Gestione Mesh
        if (auto mesh = std::dynamic_pointer_cast<eng::Mesh>(node)) {
            renderMesh(mesh, globalMatrix, currentLOD);
        }

        // 3. Ricorsione sui figli
        for (const auto& child : node->get_children()) {
            renderOvoNode(child, globalMatrix, currentLOD);
        }
    }
    /**
   * @brief Configura una luce OpenGL a partire da un nodo Light.
   *
   * Supporta:
   * - Luci direzionali
   * - Luci puntiformi
   * - Spot light
   *
   * @param light Puntatore alla luce
   * @param globalMatrix Trasformazione globale del nodo luce
   */
    void Renderer::processLight(std::shared_ptr<eng::Light> light, const glm::mat4& globalMatrix) {
        int lightID = GL_LIGHT1 + currentLightCount;

        if (lightID > GL_LIGHT7) {
            std::cout << "[WARNING] Too many lights! Ignoring extra lights." << std::endl;
            return;
        }

        glEnable(lightID);

        // Impostazioni Comuni (Colori)
        glLightfv(lightID, GL_DIFFUSE, glm::value_ptr(glm::vec4(light->get_diffuse_color(), 1.0f)));
        glLightfv(lightID, GL_SPECULAR, glm::value_ptr(glm::vec4(light->get_specular_color(), 1.0f)));

        // Impostazioni Specifiche per Tipo
        if (auto dirLight = std::dynamic_pointer_cast<eng::DirectionalLight>(light)) {
            glm::mat3 rotationMatrix = glm::mat3(globalMatrix);
            glm::vec3 globalDir = rotationMatrix * dirLight->get_direction();
            glm::vec4 pos(globalDir.x, globalDir.y, globalDir.z, 0.0f);

            glLightfv(lightID, GL_POSITION, glm::value_ptr(pos));
            glLightf(lightID, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(lightID, GL_LINEAR_ATTENUATION, 0.0f);
            glLightf(lightID, GL_QUADRATIC_ATTENUATION, 0.0f);
            glLightf(lightID, GL_SPOT_CUTOFF, 180.0f);
        }
        else if (auto pointLight = std::dynamic_pointer_cast<eng::PointLight>(light)) {
            glm::vec4 pos = globalMatrix * glm::vec4(0, 0, 0, 1.0f);
            glLightfv(lightID, GL_POSITION, glm::value_ptr(pos));

            float r = pointLight->get_radius();
            glLightf(lightID, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(lightID, GL_LINEAR_ATTENUATION, 4.5f / r);
            glLightf(lightID, GL_QUADRATIC_ATTENUATION, 75.0f / (r * r));
            glLightf(lightID, GL_SPOT_CUTOFF, 180.0f);
        }
        else if (auto spotLight = std::dynamic_pointer_cast<eng::SpotLight>(light)) {
            glm::vec4 pos = globalMatrix * glm::vec4(0, 0, 0, 1.0f);
            glLightfv(lightID, GL_POSITION, glm::value_ptr(pos));

            glm::mat3 rotationMatrix = glm::mat3(globalMatrix);
            glm::vec3 globalDir = rotationMatrix * spotLight->get_direction();
            glLightfv(lightID, GL_SPOT_DIRECTION, glm::value_ptr(globalDir));

            glLightf(lightID, GL_SPOT_CUTOFF, spotLight->get_cutoff());
            glLightf(lightID, GL_SPOT_EXPONENT, spotLight->get_exponent());
        }

        currentLightCount++;
    }
    /**
    * @brief Configura il materiale corrente in OpenGL.
    *
    * Imposta proprietà di illuminazione e texture.
    *
    * @param mat Materiale da applicare (può essere nullo)
    */
    void Renderer::setupMaterial(std::shared_ptr<eng::Material> mat) {
        if (!mat) {
            glDisable(GL_TEXTURE_2D);
            return;
        }

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(glm::vec4(mat->get_ambient_color(), 1.0f)));
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(glm::vec4(mat->get_diffuse_color(), 1.0f)));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(glm::vec4(mat->get_specular_color(), 1.0f)));
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat->get_shininess());

        if (mat->get_texture()) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, mat->get_texture()->get_texture_id());
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        else {
            glDisable(GL_TEXTURE_2D);
        }
    }
    /**
   * @brief Renderizza una mesh triangolare.
   *
   * Disegna i triangoli utilizzando i vertici, normali e coordinate UV
   * del livello di dettaglio richiesto.
   *
   * @param mesh Mesh da renderizzare
   * @param globalMatrix Trasformazione globale della mesh
   * @param currentLOD Livello di dettaglio
   */
    void Renderer::renderMesh(std::shared_ptr<eng::Mesh> mesh, const glm::mat4& globalMatrix, int currentLOD) {
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(globalMatrix));

        // Deleghiamo il setup del materiale
        setupMaterial(mesh->get_material());

        const auto& verts = mesh->get_vertices(currentLOD);
        const auto& norms = mesh->get_normals(currentLOD);
        const auto& uvs = mesh->get_uvs(currentLOD);
        const auto& faces = mesh->get_faces(currentLOD);

        glBegin(GL_TRIANGLES);
        for (const auto& face : faces) {
            uint32_t i0 = std::get<0>(face), i1 = std::get<1>(face), i2 = std::get<2>(face);

            if (i0 < norms.size()) glNormal3fv(glm::value_ptr(norms[i0]));
            if (i0 < uvs.size()) glTexCoord2fv(glm::value_ptr(uvs[i0]));
            if (i0 < verts.size()) glVertex3fv(glm::value_ptr(verts[i0]));

            if (i1 < norms.size()) glNormal3fv(glm::value_ptr(norms[i1]));
            if (i1 < uvs.size()) glTexCoord2fv(glm::value_ptr(uvs[i1]));
            if (i1 < verts.size()) glVertex3fv(glm::value_ptr(verts[i1]));

            if (i2 < norms.size()) glNormal3fv(glm::value_ptr(norms[i2]));
            if (i2 < uvs.size()) glTexCoord2fv(glm::value_ptr(uvs[i2]));
            if (i2 < verts.size()) glVertex3fv(glm::value_ptr(verts[i2]));
        }
        glEnd();
        glPopMatrix();
    }
 

    
       /**
       * @brief Calcola la matrice di proiezione dell'ombra su un piano.
       *
       * Implementa la proiezione planare dell'ombra data una sorgente luminosa.
       *
       * @param groundPlane Equazione del piano (Ax + By + Cz + D = 0)
       * @param lightPos Posizione della luce in coordinate omogenee
       * @return Matrice 4x4 di proiezione dell'ombra
       */
    glm::mat4 Renderer::computeShadowMatrix(glm::vec4 groundPlane, glm::vec4 lightPos) {
        float dot = glm::dot(groundPlane, lightPos);
        glm::mat4 shadowMat;
        shadowMat[0][0] = dot - lightPos.x * groundPlane.x;
        shadowMat[1][0] = 0.f - lightPos.x * groundPlane.y;
        shadowMat[2][0] = 0.f - lightPos.x * groundPlane.z;
        shadowMat[3][0] = 0.f - lightPos.x * groundPlane.w;
        shadowMat[0][1] = 0.f - lightPos.y * groundPlane.x;
        shadowMat[1][1] = dot - lightPos.y * groundPlane.y;
        shadowMat[2][1] = 0.f - lightPos.y * groundPlane.z;
        shadowMat[3][1] = 0.f - lightPos.y * groundPlane.w;
        shadowMat[0][2] = 0.f - lightPos.z * groundPlane.x;
        shadowMat[1][2] = 0.f - lightPos.z * groundPlane.y;
        shadowMat[2][2] = dot - lightPos.z * groundPlane.z;
        shadowMat[3][2] = 0.f - lightPos.z * groundPlane.w;
        shadowMat[0][3] = 0.f - lightPos.w * groundPlane.x;
        shadowMat[1][3] = 0.f - lightPos.w * groundPlane.y;
        shadowMat[2][3] = 0.f - lightPos.w * groundPlane.z;
        shadowMat[3][3] = dot - lightPos.w * groundPlane.w;
        return shadowMat;
    }



    /**
   * @brief Renderizza ricorsivamente la geometria per l'ombra di un nodo.
   *
   * Disegna solo i vertici (senza illuminazione o materiali),
   * utilizzando la matrice di schiacciamento già applicata.
   *
   * @param node Nodo da cui generare l'ombra
   * @param parentMatrix Trasformazione globale dell'ombra
   */
    //prendi i vertici di questo oggetto, moltiplicali per questa matrice 'schiacciante', colorali di nero trasparente e disegnali sopra il pavimento matrice di sopra
    void Renderer::renderShadowNode(std::shared_ptr<Node> node, glm::mat4 parentMatrix) {
        if (!node) return;
        glm::mat4 globalMatrix = parentMatrix * node->get_base_matrix();
        auto mesh = std::dynamic_pointer_cast<eng::Mesh>(node);

        if (mesh) {
            glPushMatrix();
            glLoadMatrixf(glm::value_ptr(globalMatrix));

            const auto& verts = mesh->get_vertices();
            const auto& faces = mesh->get_faces();

            glBegin(GL_TRIANGLES);
            for (const auto& face : faces) {
                if (std::get<0>(face) < verts.size()) glVertex3fv(glm::value_ptr(verts[std::get<0>(face)]));
                if (std::get<1>(face) < verts.size()) glVertex3fv(glm::value_ptr(verts[std::get<1>(face)]));
                if (std::get<2>(face) < verts.size()) glVertex3fv(glm::value_ptr(verts[std::get<2>(face)]));
            }
            glEnd();
            glPopMatrix();
        }

        for (const auto& child : node->get_children()) {
            renderShadowNode(child, globalMatrix);
        }
    }
     /**
     * @brief Cerca ricorsivamente un nodo per nome nella scena.
     *
     * @param current Nodo corrente della ricerca
     * @param name Nome del nodo da trovare
     * @return Puntatore al nodo trovato oppure nullptr
     */
    std::shared_ptr<Node> findNodeForShadow(std::shared_ptr<Node> current, const std::string& name) {
        if (!current) return nullptr;
        if (current->get_name() == name) return current;
        for (auto& child : current->get_children()) {
            auto res = findNodeForShadow(child, name);
            if (res) return res;
        }
        return nullptr;
    }

    void Renderer::renderShadows(std::shared_ptr<Node> root, const std::vector<ShadowRequest>& shadows, const glm::mat4& viewMatrix) {
        if (shadows.empty() || !root) return;

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);


        // --- INIZIO MAGIA DELLO STENCIL BUFFER ---
        glEnable(GL_STENCIL_TEST);

        // Diciamo a OpenGL: "Disegna il pixel SOLO SE il valore dello stencil è diverso da 1"
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

        // Diciamo a OpenGL: "Dopo aver disegnato il pixel dell'ombra, scrivi 1 nello stencil, 
        // così nessun altro triangolo potrà disegnarci sopra"
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        // Colore dell'ombra (Nera semi-trasparente)
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

        // Parametri matematici per il piano
        glm::vec4 groundPlane(0.0f, 1.0f, 0.0f, 4.95f);
        glm::mat4 sceneTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, -40.0f));

        // Ciclo su tutte le richieste di ombra
        for (const auto& req : shadows) {

            // 1. Cerchiamo il nodo richiesto ("Table", "Pikachu", ecc.)
            std::shared_ptr<Node> targetNode = findNodeForShadow(root, req.nodeName);
            if (!targetNode) continue; // Se non lo trova, salta

            // 2. Calcoliamo la matrice dell'ombra
            glm::vec4 lightPos(req.lightPosition, 1.0f);
            glm::mat4 shadowMatrix = computeShadowMatrix(groundPlane, lightPos);

            // 3. Ordine di moltiplicazione: Camera * Ombra * Posizione nel mondo
            glm::mat4 finalMatrix = viewMatrix * shadowMatrix * sceneTransform;

            // 4. Disegniamo finalmente l'ombra!
            renderShadowNode(targetNode, finalMatrix);
        }

        // Ripristiniamo lo stato di OpenGL
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_STENCIL_TEST);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
}
