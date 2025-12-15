#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include "engine.h"


// Macro per rendere i messaggi di test più leggibili
#define TEST_START(name) std::cout << "[RUNNING] " << name << " ... ";
#define TEST_PASS() std::cout << "OK" << std::endl;

void test_singleton() {
    TEST_START("Singleton Pattern");
    Eng::Base& instance1 = Eng::Base::getInstance();
    Eng::Base& instance2 = Eng::Base::getInstance();

    // Verifica che l'indirizzo di memoria sia identico
    assert(&instance1 == &instance2 && "Errore: Singleton restituisce istanze diverse!");
    TEST_PASS();
}

void test_initialization(int argc, char* argv[]) {
    TEST_START("Engine Initialization");
    Eng::Base& engine = Eng::Base::getInstance();

    // Primo init deve tornare true
    bool res1 = engine.init("Test Window", 800, 600, argc, argv);
    assert(res1 == true && "Errore: Init fallito la prima volta");

    // Secondo init deve tornare false (già inizializzato)
    bool res2 = engine.init("Test Window 2", 800, 600, argc, argv);
    assert(res2 == false && "Errore: Init dovrebbe fallire se chiamato due volte");

    TEST_PASS();
}

void test_orb_limit() {
    TEST_START("Orb Creation Limit (Max 7)");
    Eng::Base& engine = Eng::Base::getInstance();

    // Reset ipotetico (assumiamo di partire da 0 dopo init)
    // Aggiungiamo 7 sfere
    for (int i = 0; i < 7; i++) {
        engine.createOrb(0.0f, 0.0f, 0.0f);
    }

    assert(engine.getOrbCount() == 7 && "Errore: Dovrebbero esserci 7 sfere");

    // Proviamo ad aggiungerne un'ottava (dovrebbe essere bloccata)
    engine.createOrb(10.0f, 10.0f, 10.0f);

    assert(engine.getOrbCount() == 7 && "Errore: Il motore ha permesso più di 7 luci!");

    TEST_PASS();
}

void test_scene_graph_manipulation() {
    TEST_START("Scene Graph: SetParent");
    Eng::Base& engine = Eng::Base::getInstance();

    auto root = engine.getSceneRoot();
    if (!root) {
        std::cout << "(Mocking Scene) ";
    }

    // --- CORREZIONE: Usiamo costruttore vuoto + set_name ---

    // 1. Padre A
    auto parentA = std::make_shared<eng::Node>();
    parentA->set_name("PadreA");

    // 2. Padre B
    auto parentB = std::make_shared<eng::Node>();
    parentB->set_name("PadreB");

    // 3. Figlio
    auto child = std::make_shared<eng::Node>();
    child->set_name("Figlio");

    // Attacchiamo tutto alla root se esiste
    if (root) {
        root->add_child(parentA);
        root->add_child(parentB);
        parentA->add_child(child); // Il figlio parte sotto A

        // Verifica stato iniziale
        assert(engine.getNode("Figlio") != nullptr);

        // ESECUZIONE TEST: Sposta 'Figlio' da 'PadreA' a 'PadreB'
        engine.setParent("Figlio", "PadreB");

        // VERIFICHE
        // Cerchiamo manualmente nelle liste dei figli
        bool foundInB = false;
        for (auto c : parentB->get_children()) {
            if (c->get_name() == "Figlio") foundInB = true;
        }

        bool foundInA = false;
        for (auto c : parentA->get_children()) {
            if (c->get_name() == "Figlio") foundInA = true;
        }

        assert(foundInB == true && "Errore: Il figlio non è stato spostato in PadreB");
        assert(foundInA == false && "Errore: Il figlio è ancora in PadreA (duplicato!)");
    }
    else {
        std::cout << "[SKIP] Root non disponibile (OVO non caricato), impossibile testare reparenting completo." << std::endl;
    }

    TEST_PASS();
}

void test_texture_fail_safe() {
    TEST_START("Texture Load on Invalid Object");
    Eng::Base& engine = Eng::Base::getInstance();

    try {
        engine.loadTexture("fake.png", "OggettoInesistenteXYZ");
    }
    catch (...) {
        assert(false && "Errore: loadTexture ha lanciato un'eccezione su oggetto inesistente");
    }
    TEST_PASS();
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "      ENGINE AUTOMATED UNIT TESTS       " << std::endl;
    std::cout << "========================================" << std::endl;

    test_singleton();
    test_initialization(argc, argv);
    test_orb_limit();
    test_texture_fail_safe();
    test_scene_graph_manipulation();

    std::cout << "========================================" << std::endl;
    std::cout << "   TUTTI I TEST SUPERATI CON SUCCESSO   " << std::endl;
    std::cout << "========================================" << std::endl;

    // Chiudiamo tutto
    Eng::Base::getInstance().free();

    // Pausa finale per leggere
    std::cout << "Premi INVIO per uscire...";
    std::cin.get();

    return 0;
}