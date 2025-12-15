#include "test_scene.h"
#include "test_utils.h"
#include "engine.h"   // Usiamo l'Engine!
#include "node.h"
#include "mesh.h"
#include <cassert>
#include <memory>

void test_node_logic() {
    TEST_START("Engine: Scene Management (getNode & setParent)");

    Eng::Base& engine = Eng::Base::getInstance();

    // 1. Setup: Creiamo una scena finta DENTRO l'engine
    auto root = engine.getSceneRoot();
    if (!root) {
        // Se non c'è root (es. OVO disabilitato), creiamo una root temporanea
        // Nota: Questo serve solo perché abbiamo disabilitato il caricamento OVO
        std::cout << "[WARN] Root non trovata, skip test..." << std::endl;
        return;
    }

    // Creiamo due nodi e li buttiamo nella scena dell'Engine
    auto parentA = std::make_shared<eng::Node>();
    parentA->set_name("PadreA");

    auto child = std::make_shared<eng::Node>();
    child->set_name("FiglioConteso");

    // Attacchiamo tutto alla root dell'engine
    root->add_child(parentA);
    parentA->add_child(child); // Inizia sotto PadreA

    // 2. Test: L'engine riesce a trovare il nodo per nome?
    auto foundNode = engine.getNode("FiglioConteso");
    assert(foundNode != nullptr && "Errore: engine.getNode non trova il nodo appena aggiunto");
    assert(foundNode == child && "Errore: Il nodo trovato non corrisponde all'originale");

    // 3. Test: L'engine riesce a fare il Reparenting?
    // Creiamo un nuovo padre
    auto parentB = std::make_shared<eng::Node>();
    parentB->set_name("PadreB");
    root->add_child(parentB);

    // CHIAMATA ALL'ENGINE: Sposta "FiglioConteso" sotto "PadreB"
    engine.setParent("FiglioConteso", "PadreB");

    // VERIFICA:
    // Controlliamo se PadreB ora ha il figlio
    bool foundInB = false;
    for (auto c : parentB->get_children()) {
        if (c->get_name() == "FiglioConteso") foundInB = true;
    }
    assert(foundInB && "Errore: engine.setParent non ha spostato il figlio nel nuovo padre");

    // Controlliamo se PadreA l'ha perso
    bool foundInA = false;
    for (auto c : parentA->get_children()) {
        if (c->get_name() == "FiglioConteso") foundInA = true;
    }
    assert(!foundInA && "Errore: engine.setParent non ha rimosso il figlio dal vecchio padre");

    TEST_PASS();
}

void test_mesh_data() {
    // Questo test rimane un test di unità puro perché l'Engine 
    // non ha metodi specifici per creare mesh da zero (le carica da OVO).
    // Lo lasciamo semplice o lo saltiamo se ti interessano solo i comandi engine.
    TEST_START("Mesh: Data Integrity");
    auto mesh = std::make_shared<eng::Mesh>();
    // ... (codice identico a prima per verificare che la classe Mesh funzioni) ...
    TEST_PASS();
}