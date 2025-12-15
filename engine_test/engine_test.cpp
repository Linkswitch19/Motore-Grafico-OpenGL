#include <iostream>
#include "test_utils.h"
#include "test_scene.h"
#include "engine.h"
#include "test_rendering.h"
// Includi i nostri nuovi file di test


int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "      ENGINE AUTOMATED UNIT TESTS       " << std::endl;
    std::cout << "========================================" << std::endl;

    // 1. Inizializza Engine (necessario per OpenGL/GLUT context se usato)
    Eng::Base::getInstance().init("Test Runner", 800, 600, argc, argv);

    // --- GRUPPO 1: SCENE GRAPH ---
    test_node_logic();
    test_mesh_data();

    // --- GRUPPO 2: RENDERING & MATH ---
    test_material();
    test_camera_math();
    test_lights_compilation();

    std::cout << "========================================" << std::endl;
    std::cout << "   TUTTI I TEST SUPERATI CON SUCCESSO   " << std::endl;
    std::cout << "========================================" << std::endl;

    Eng::Base::getInstance().free();

    std::cout << "Premi INVIO per uscire...";
    std::cin.get();

    return 0;
}