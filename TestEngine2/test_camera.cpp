// test_camera.cpp
#include "test_camera.h"
#include "camera.h"  // La classe da testare
#include "engine.h"  // Per il test di integrazione
#include <iostream>
#include <cassert>
#include <cmath>
#include <glm/glm.hpp>

// --- Helper Locali per i confronti (visibili solo in questo file) ---
static bool is_close(float a, float b, float epsilon = 0.001f) {
    return std::abs(a - b) < epsilon;
}

static bool is_vec_close(glm::vec3 a, glm::vec3 b, float epsilon = 0.001f) {
    return glm::distance(a, b) < epsilon;
}

static void print_header(const char* name) {
    std::cout << "[RUNNING] " << name << " ... ";
}

static void print_pass() {
    std::cout << "OK" << std::endl;
}

// --- Implementazione Test ---

void test_camera_math() {
    print_header("Camera: Math & Vectors (Unit Test)");

    // 1. Setup: Posizione 0,0,0 - Guarda verso -Z (-90 Yaw)
    glm::vec3 startPos(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    eng::Camera cam(startPos, up, -90.0f, 0.0f);

    // Verifica stato iniziale
    assert(is_vec_close(cam.Position, glm::vec3(0, 0, 0)) && "Errore: Posizione iniziale errata");
    // Front deve essere (0,0,-1)
    assert(is_vec_close(cam.Front, glm::vec3(0, 0, -1)) && "Errore: Front vector iniziale errato");

    // 2. Movimento FORWARD (lungo -Z)
    // Speed = 2.5f (default). DeltaTime = 1.0f. Spostamento atteso: 2.5 unità verso -Z.
    cam.ProcessKeyboard(eng::FORWARD, 1.0f);
    assert(cam.Position.z < -0.1f && "Errore: La camera non è avanzata (Z non è diminuita)");

    // 3. Movimento RIGHT (lungo +X)
    float oldX = cam.Position.x;
    cam.ProcessKeyboard(eng::RIGHT, 1.0f);
    assert(cam.Position.x > oldX && "Errore: La camera non si è mossa a destra");

    // 4. Rotazione Yaw (Gira a destra di 90 gradi)
    // Da -90 (guarda -Z) passa a 0 (guarda +X)
    cam.RotateBy(90.0f);
    // Verifichiamo che il vettore Front abbia una forte componente X positiva
    assert(cam.Front.x > 0.9f && "Errore: Rotazione Yaw fallita (Front vector errato)");

   

    print_pass();
}

void test_camera_engine_integration() {
    print_header("Camera: Engine Integration");
    Eng::Base& engine = Eng::Base::getInstance();

    // Verifichiamo che i metodi wrapper dell'Engine chiamino la camera senza crashare
    try {
        // Reset
        engine.setCameraPosition(glm::vec3(10, 10, 10), glm::vec3(0, 1, 0), -90.0f, 0.0f);

        // Input simulati
        engine.moveCamera(eng::BACKWARD, 0.1f);
        engine.rotateCamera(45.0f);

        // Se siamo qui senza SegFault, l'integrazione base funziona
    }
    catch (...) {
        assert(false && "Errore: Eccezione durante l'uso della Camera tramite Engine");
    }

    print_pass();
}