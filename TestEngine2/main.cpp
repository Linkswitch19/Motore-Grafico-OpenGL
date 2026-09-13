// main.cpp
#include <iostream>
#include "engine.h"
#include "test_engine.h" // Includiamo i nostri test
#include "test_camera.h"
#include "test_ovo.h"
#include "test_texture.h"

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "      ENGINE AUTOMATED UNIT TESTS       " << std::endl;
    std::cout << "========================================" << std::endl;

    // 1. Test Singleton (pre-init)
    test_singleton();

    // 2. Test Inizializzazione (Init reale)
    //test_initialization(argc, argv); non si puo su pipeline

    test_camera_math();
    test_light_limit_logic();
    // --- I/O & Parsing ---
    test_ovo_parsing(); // <--- Eseguiamo il test del parser qui
    //test_texture_system(); 

    // 3. Test Robustezza & Risorse
    test_robustness();
   

    // 4. Test Scena e Logica
    test_scene_graph_deep_search();
    test_scene_graph_manipulation();
    // --- GRUPPO 4: Integrazione ---
    test_camera_engine_integration();

    std::cout << "========================================" << std::endl;
    std::cout << "   TUTTI I TEST SUPERATI CON SUCCESSO   " << std::endl;
    std::cout << "========================================" << std::endl;

    // Pulizia finale
    Eng::Base::getInstance().free();

  

    return 0;
}