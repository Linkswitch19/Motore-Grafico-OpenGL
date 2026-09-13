#pragma once

// Includi le tue macro (ENG_API, DEBUG, ecc.) e la forzatura per la GPU dedicata
#include "lib.h"

// Dipendenze pubbliche strettamente necessarie per il client (main.cpp)
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

// Header del tuo progetto che servono come parametri e ritorni pubblici
#include "node.h"
#include "textHUD.h"
#include "enums.h"

typedef void (*KeyboardCallback)(unsigned char key, int x, int y);
namespace Eng {

    /**
     * @class Base
     * @brief Core Engine class (Singleton Facade).
     * Gestisce l'inizializzazione, il loop principale e smista le chiamate ai vari sottosistemi.
     */
    class ENG_API Base {
    private:
        // PIMPL: Nasconde tutta l'implementazione e i manager interni!
        struct Reserved;
        std::unique_ptr<Reserved> reserved;

        // Costruttori privati per il pattern Singleton
        Base();
        ~Base();

        // Evita la copia del Singleton
        Base(const Base&) = delete;
        Base& operator=(const Base&) = delete;

        // Callback interne
        void initialize();
        void updateAnimation(float deltaTime);

    public:
        // Variabili pubbliche
        int window_width = 800;
        int window_height = 600;
        bool fps_show = false;
        int fps = 0;
        int frames = 0;
        int detail = 1; // LOD level
        int prev_time = 0;

        glm::mat4 perspective;
        glm::mat4 ortho;

        KeyboardCallback keyboardCallback = nullptr;

        // --- SINGLETON ---
        static Base& getInstance();

        // --- LIFECYCLE ---
        bool init(const char* title, unsigned int width, unsigned int height, int argc, char* argv[]);
        void startLoop();
        bool free();
        void update();

        // --- WINDOW & RENDER STATE ---
        void clearWindow();
        void setBackgroundColor(float r, float g, float b);
        void swapBuffers();

        // --- CALLBACKS (Richieste da GLUT) ---
        void onDisplay();
        void onKeyPressed(unsigned char key, int x, int y);
        void setKeyboardCallback(KeyboardCallback cb);
        static void timerCallback(int value);
        void reshapeCallback(int width, int height);

        // --- CAMERA ---
        void moveCamera(eng::Camera_Movement direction, float deltaTime);
        void rotateCamera(float angle);
        void setCameraPosition(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch);
        void toggleCameraProjection();
        void pitchCamera(float angle);


        // --- SCENA E NODI ---
        std::shared_ptr<eng::Node> getNode(const std::string& name);
        std::shared_ptr<eng::Node> getSceneRoot();
        void setParent(const std::string& childName, const std::string& parentName);
        float getMeshHeight(const std::string& nodeName);

        // --- LUCI E OMBRE ---
        void createOrb(float x, float y, float z);
        void addLight(float x, float y, float z, float r, float g, float b, float a);
        void addShadow(const std::string& nodeName, float x, float y, float z);
        int getOrbCount() const;

        // --- HUD E TESTO ---
        void addText(const eng::TextHUD& textObject);
        void setMessage(const std::string& msg);
        void clearHUD();



    };

}