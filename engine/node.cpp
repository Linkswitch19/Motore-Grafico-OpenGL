#include "node.h"

namespace eng {

    /**
     * @brief Costruttore predefinito della classe Node.
     * Inizializza la matrice di trasformazione locale (baseMatrix) come matrice identità (1.0f),
     * il che significa nessuna traslazione, rotazione o scala iniziale.
     */
    ENG_API Node::Node() : baseMatrix(1.0f) {}

    /**
     * @brief Imposta il nome identificativo del nodo.
     * Il nome è cruciale per poter cercare e recuperare i nodi all'interno della scena
     * (ad esempio tramite la funzione getNode() del SceneManager).
     * @param n La stringa contenente il nome da assegnare al nodo.
     */
    void ENG_API Node::set_name(const std::string& n) {
        name = n;
    }

    /**
     * @brief Restituisce il nome attuale del nodo.
     * @return std::string Il nome del nodo.
     */
    std::string ENG_API Node::get_name() const {
        return name;
    }

    /**
     * @brief Imposta la matrice di trasformazione locale (Base Matrix).
     * Questa matrice definisce la posizione, rotazione e scala del nodo in modo relativo
     * al suo nodo padre. Moltiplicandola per la matrice del padre si ottiene la matrice globale.
     * @param m La nuova matrice di trasformazione 4x4.
     */
    void ENG_API Node::set_base_matrix(const glm::mat4& m) {
        baseMatrix = m;
    }

    /**
     * @brief Restituisce la matrice di trasformazione locale del nodo.
     * @return glm::mat4 La matrice 4x4 relativa allo spazio locale del nodo.
     */
    glm::mat4 ENG_API Node::get_base_matrix() const {
        return baseMatrix;
    }

    /**
     * @brief Aggiunge un nuovo nodo figlio a questa gerarchia.
     * Inserisce il nodo passato come argomento nella lista dei figli e, contemporaneamente,
     * imposta il puntatore 'parent' del figlio affinché punti a questo nodo.
     * @param child Puntatore condiviso (shared_ptr) al nodo figlio da aggiungere.
     */
    void ENG_API Node::add_child(std::shared_ptr<Node> child) {
        if (child) {
            children.push_back(child);
            child->parent = this;
        }
    }

    /**
     * @brief Restituisce la lista di tutti i nodi figli collegati a questo nodo.
     * @return std::vector<std::shared_ptr<Node>>& Riferimento al vettore contenente i figli.
     */
    std::vector<std::shared_ptr<Node>>& Node::get_children() {
        return children;
    }

}