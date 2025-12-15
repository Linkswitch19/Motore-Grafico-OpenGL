#include "test_engine.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

// Macro locali per formattazione (visibili solo in questo file)
#define TEST_START(name) std::cout << "[RUNNING] " << name << " ... ";
#define TEST_PASS() std::cout << "OK" << std::endl;

void test_singleton() {
    TEST_START("Singleton Pattern");
    Eng::Base& instance1 = Eng::Base::getInstance();
    Eng::Base& instance2 = Eng::Base::getInstance();
    assert(&instance1 == &instance2 && "Errore: Singleton restituisce istanze diverse!");
    TEST_PASS();
}

void test_initialization(int argc, char* argv[]) {
    TEST_START("Engine Initialization");
    Eng::Base& engine = Eng::Base::getInstance();

    bool res1 = engine.init("Test Window", 800, 600, argc, argv);
    assert(res1 == true && "Errore: Init fallito la prima volta");

    bool res2 = engine.init("Test Window 2", 800, 600, argc, argv);
    assert(res2 == false && "Errore: Init dovrebbe fallire se chiamato due volte");
    TEST_PASS();
}

void test_robustness() {
    TEST_START("Robustness (Invalid Inputs)");
    Eng::Base& engine = Eng::Base::getInstance();

    // Test 1: Reparenting su stringhe vuote o inesistenti
    try {
        engine.setParent("GhostNode", "GhostParent"); // Non deve crashare
        engine.setParent("", "");
    }
    catch (...) {
        assert(false && "Errore: Crash su nomi non validi in setParent");
    }

    // Test 2: Texture su oggetto inesistente
    try {
        engine.loadTexture("fake.png", "");
    }
    catch (...) {
        assert(false && "Errore: Crash su nome vuoto in loadTexture");
    }
    TEST_PASS();
}

void test_orb_limit() {
    TEST_START("Orb Creation Limit (Max 7)");
    Eng::Base& engine = Eng::Base::getInstance();

    // Assumiamo reset o stato iniziale. Aggiungiamo 7 sfere.
    int currentOrbs = engine.getOrbCount();
    for (int i = 0; i < (7 - currentOrbs); i++) {
        engine.createOrb(0.0f, 0.0f, 0.0f);
    }

    // Proviamo ad aggiungerne un'altra oltre il limite
    engine.createOrb(10.0f, 10.0f, 10.0f);

    assert(engine.getOrbCount() <= 7 && "Errore: Il motore ha permesso troppe luci!");
    TEST_PASS();
}

void test_texture_fail_safe() {
    TEST_START("Texture Load on Invalid Object");
    Eng::Base& engine = Eng::Base::getInstance();
    try {
        engine.loadTexture("fake.png", "OggettoInesistenteXYZ");
    }
    catch (...) {
        assert(false && "Errore: Eccezione non gestita in loadTexture");
    }
    TEST_PASS();
}

void test_scene_graph_deep_search() {
    TEST_START("Scene Graph: Recursive Search");
    Eng::Base& engine = Eng::Base::getInstance();
    auto root = engine.getSceneRoot();

    if (root) {
        // Creiamo catena: Root -> Nonno -> Papà -> Nipote
        auto grandpa = std::make_shared<eng::Node>(); grandpa->set_name("Nonno_Test");
        auto father = std::make_shared<eng::Node>(); father->set_name("Papa_Test");
        auto grandson = std::make_shared<eng::Node>(); grandson->set_name("Nipote_Test");

        root->add_child(grandpa);
        grandpa->add_child(father);
        father->add_child(grandson);

        // Cerchiamo direttamente il nipote tramite l'engine
        auto found = engine.getNode("Nipote_Test");
        assert(found != nullptr && "Errore: getNode non trova i nodi in profondità");
        assert(found->get_name() == "Nipote_Test" && "Errore: Nome nodo non corrispondente");
    }
    else {
        std::cout << "(Root non init) ";
    }
    TEST_PASS();
}

void test_scene_graph_manipulation() {
    TEST_START("Scene Graph: SetParent");
    Eng::Base& engine = Eng::Base::getInstance();
    auto root = engine.getSceneRoot();

    if (root) {
        auto parentA = std::make_shared<eng::Node>(); parentA->set_name("PadreA");
        auto parentB = std::make_shared<eng::Node>(); parentB->set_name("PadreB");
        auto child = std::make_shared<eng::Node>(); child->set_name("FiglioMobile");

        root->add_child(parentA);
        root->add_child(parentB);
        parentA->add_child(child);

        // Sposta
        engine.setParent("FiglioMobile", "PadreB");

        // Verifica
        bool foundInB = false;
        for (auto c : parentB->get_children()) if (c->get_name() == "FiglioMobile") foundInB = true;

        bool foundInA = false;
        for (auto c : parentA->get_children()) if (c->get_name() == "FiglioMobile") foundInA = true;

        assert(foundInB == true && "Errore: Non spostato in B");
        assert(foundInA == false && "Errore: Rimasto in A");
    }
    TEST_PASS();
}


