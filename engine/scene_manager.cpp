#include "scene_manager.h"
#include "ovo_parser.h"
#include <iostream>
namespace eng {

    /**
     * @brief Carica una scena a partire da un file in formato OVO.
     * * @param filepath Il percorso del file OVO da caricare.
     * @return true Se il file è stato caricato e parsato con successo.
     * @return false Se si è verificato un errore durante il caricamento (es. file non trovato).
     */
    bool SceneManager::loadScene(const std::string& filepath) {
        sceneRoot = eng::OVOParser::from_file(filepath);
        if (!sceneRoot) {
            std::cout << "[ERROR] Failed to load OVO file: " << filepath << std::endl;
            return false;
        }
        return true;
    }
    /**
     * @brief Restituisce il nodo radice (root) della scena corrente.
     * * @return std::shared_ptr<Node> Puntatore condiviso al nodo radice.
     */
    std::shared_ptr<Node> SceneManager::getRoot() const {
        return sceneRoot;
    }
    /**
     * @brief Cerca ricorsivamente un nodo per nome partendo da un nodo specifico.
     * * @param current Il nodo da cui far partire la ricerca ricorsiva.
     * @param name Il nome del nodo da cercare.
     * @return std::shared_ptr<Node> Puntatore al nodo se trovato, altrimenti nullptr.
     */
    std::shared_ptr<Node> SceneManager::findNodeRecursive(std::shared_ptr<Node> current, const std::string& name) {
        if (!current) return nullptr;
        if (current->get_name() == name) return current;
        for (auto& child : current->get_children()) {
            auto res = findNodeRecursive(child, name);
            if (res) return res;
        }
        return nullptr;
    }
    /**
     * @brief Cerca e restituisce un nodo all'interno dell'intera scena dato il suo nome.
     * * @param name Il nome del nodo da cercare.
     * @return std::shared_ptr<Node> Puntatore al nodo richiesto, oppure nullptr se non esiste nella scena.
     */
    std::shared_ptr<Node> SceneManager::getNode(const std::string& name) {
        if (!sceneRoot) return nullptr;
        return findNodeRecursive(sceneRoot, name);
    }
    /**
     * @brief Cerca ricorsivamente il nodo genitore di un nodo specifico (figlio).
     * * @param current Il nodo da cui iniziare la ricerca.
     * @param childName Il nome del nodo figlio di cui si vuole trovare il padre.
     * @return std::shared_ptr<Node> Puntatore al nodo padre, oppure nullptr se non trovato.
     */
    std::shared_ptr<Node> SceneManager::findParentOf(std::shared_ptr<Node> current, const std::string& childName) {
        if (!current) return nullptr;
        for (auto& child : current->get_children()) {
            if (child->get_name() == childName) return current;
            auto result = findParentOf(child, childName);
            if (result) return result;
        }
        return nullptr;
    }
    /**
     * @brief Modifica la gerarchia della scena spostando un nodo sotto un nuovo nodo padre.
     * * Rimuove il nodo identificato da childName dal suo genitore attuale e lo
     * aggiunge alla lista dei figli del nodo identificato da parentName.
     * * @param childName Il nome del nodo da spostare.
     * @param parentName Il nome del nuovo nodo padre che accoglierà il figlio.
     */

    void SceneManager::setParent(const std::string& childName, const std::string& parentName) {
        if (!sceneRoot) return;
        auto currentParent = findParentOf(sceneRoot, childName);
        if (!currentParent) return;

        std::shared_ptr<Node> childNode = nullptr;
        auto& siblings = currentParent->get_children();

        for (auto it = siblings.begin(); it != siblings.end(); ) {
            if ((*it)->get_name() == childName) {
                childNode = *it;
                it = siblings.erase(it);
                break;
            }
            else {
                ++it;
            }
        }

        if (!childNode) return;

        std::shared_ptr<Node> newParentNode = getNode(parentName);
        if (!newParentNode) {
            currentParent->add_child(childNode); // Revert
            std::cout << "[ERROR] New parent " << parentName << " not found!" << std::endl;
            return;
        }

        newParentNode->add_child(childNode);
    }
    /**
     * @brief Calcola e restituisce l'altezza assoluta di una mesh specifica.
     * * L'altezza viene calcolata trovando la differenza tra la coordinata Y massima
     * e la coordinata Y minima dei vertici che compongono la mesh.
     * * @param nodeName Il nome del nodo mesh di cui calcolare l'altezza.
     * @return float L'altezza della mesh. Restituisce 0.0f se il nodo non esiste o non è una mesh.
     */
    float SceneManager::getMeshHeight(const std::string& nodeName) {
        auto node = getNode(nodeName);
        if (!node) return 0.0f;

        auto mesh = std::dynamic_pointer_cast<eng::Mesh>(node);
        if (!mesh) return 0.0f;

        float minY = 99999.0f;
        float maxY = -99999.0f;

        for (const auto& vertex : mesh->get_vertices()) {
            if (vertex.y < minY) minY = vertex.y;
            if (vertex.y > maxY) maxY = vertex.y;
        }
        return (maxY - minY);
    }

}
