/**
 * @file main.cpp
 * @brief Client application using the graphics engine to implement the Tower of Hanoi game.
 */

 //=================
 // Includes
 //=================

#include "engine.h"

#include <iostream>
#include <vector>
#include <list>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;
#define _CRT_SECURE_NO_WARNINGS

//=================
// Configuration
//=================

/** X positions of the three poles in world coordinates */
float POLE_X_POSITIONS[3] = { -20.0f, 0.0f, 20.0f };

/** Vertical thickness of a single disk */
float DISK_HEIGHT = 0.0f;

/** Base height of the poles from the ground */
float BASE_HEIGHT = 0.0f;

//=================
// Hanoi Game Class
//=================

/**
 * @class HanoiGame
 * @brief Handles game logic and synchronization with the graphical scene.
 */
class HanoiGame {

struct Move {
        int sourcePole;
        int destPole;
};
private:
    /** Logical state: each pole stores disk IDs (1 = smallest, 7 = largest) */
    std::vector<int> poles[3];

    /** Scene nodes corresponding to the disks */
    std::shared_ptr<eng::Node> diskNodes[7];

    /** Names of the pole nodes in the scene */
    std::string poleNodeNames[3] = { "Palo1", "Palo2", "Palo3" };

    /** Selected source pole (-1 means none selected) */
    int selectedSourcePole = -1;

    std::vector<Move> history; // Tiene traccia delle mosse fatte
    std::vector<Move> redoList; // Tiene traccia delle mosse annullate

public:
    /**
     * @brief Initializes the game state and performs automatic scene calibration.
     */
    void init() {
        auto& engine = Eng::Base::getInstance();

        for (int i = 0; i < 7; i++) {
            std::string name = "Disco" + std::to_string(i + 1);
            diskNodes[i] = engine.getNode(name);
            if (!diskNodes[i]) {
                std::cout << "ERROR: Node " << name << " not found" << std::endl;
            }
        }


        // Chiediamo all'Engine lo spessore senza toccare la classe Mesh!
        DISK_HEIGHT = engine.getMeshHeight("Disco1");
        std::cout << "[INFO] Spessore dischi letto dall'Engine: " << DISK_HEIGHT << std::endl;

        // Automatic calibration using the lowest disk (Disco7)
        if (diskNodes[6]) {
            glm::mat4 m = diskNodes[6]->get_base_matrix();
            float startX = m[3][0];
            float startY = m[3][1];

            float distance = 40.0f;
            POLE_X_POSITIONS[0] = startX;
            POLE_X_POSITIONS[1] = startX + distance;
            POLE_X_POSITIONS[2] = startX + distance * 2.0f;
            BASE_HEIGHT = startY;
        }

        for (int i = 1; i <= 7; i++) {
            poles[0].push_back(i);
        }

        updateVisuals();
    }



    /**
     * @brief Resets the game to the initial state (all disks on the first pole).
     */
    void reset() {
        // Clear all poles
        for (int p = 0; p < 3; p++) {
            poles[p].clear();
        }

        // Add all disks back to the first pole (disk 1 to 7)
        for (int i = 1; i <= 7; i++) {
            poles[0].push_back(i);
        }

        // Reset the selection state
        selectedSourcePole = -1;
        Eng::Base::getInstance().setMessage("Game reset!");
        history.clear();
        redoList.clear();
        // Update the visual representation
        updateVisuals();
    }

    /**
     * @brief Updates disk transforms and parent relationships in the scene.
     */
    void updateVisuals() {
        auto& engine = Eng::Base::getInstance();
        const float HOVER_HEIGHT = 20.0f;

        for (int p = 0; p < 3; p++) {
            float currentY = 0.0f;

            for (size_t i = 0; i < poles[p].size(); i++) {
                int diskID = poles[p][i];
                int diskIndex = diskID - 1;

                std::string diskName = "Disco" + std::to_string(diskID);
                std::string poleName = "Palo" + std::to_string(p + 1);

                engine.setParent(diskName, poleName);

                float drawY = currentY;
                if (p == selectedSourcePole && i == poles[p].size() - 1) {
                    drawY = HOVER_HEIGHT;
                }

                if (diskNodes[diskIndex]) {
                    glm::mat4 localMat(1.0f);
                    localMat = glm::translate(localMat, glm::vec3(0.0f, drawY, 0.0f));
                    diskNodes[diskIndex]->set_base_matrix(localMat);
                }

                currentY += DISK_HEIGHT;
            }
        }
    }


    /**
     * @brief Esegue una mossa annullata (Redo)
     */
    void redoMove() {
        if (redoList.empty()) {
            Eng::Base::getInstance().setMessage("Nothing to redo!");
            return;
        }

        Move m = redoList.back();
        redoList.pop_back();

        // Eseguiamo la mossa
        int diskToMove = poles[m.sourcePole].back();
        poles[m.sourcePole].pop_back();
        poles[m.destPole].push_back(diskToMove);

        // Aggiungiamo alla storia
        history.push_back(m);

        selectedSourcePole = -1; // Deseleziona tutto
        updateVisuals();
        Eng::Base::getInstance().setMessage("Redo move.");
    }


    /**
     * @brief Annulla l'ultima mossa (Undo)
     */
    void undoMove() {
        if (history.empty()) {
            Eng::Base::getInstance().setMessage("Nothing to undo!");
            return;
        }

        Move m = history.back();
        history.pop_back();

        // Attenzione: nell'Undo, la destPole diventa la sourcePole (la mossa va al contrario)
        int diskToMove = poles[m.destPole].back();
        poles[m.destPole].pop_back();
        poles[m.sourcePole].push_back(diskToMove);

        // Aggiungiamo alla lista redo per poterla rifare
        redoList.push_back(m);

        selectedSourcePole = -1; // Deseleziona tutto
        updateVisuals();
        Eng::Base::getInstance().setMessage("Undo move.");
    }
    /**
     * @brief Handles keyboard input for game actions.
     */
    void handleInput(unsigned char key) {
        int poleIndex = -1;
        if (key == '1') poleIndex = 0;
        else if (key == '2') poleIndex = 1;
        else if (key == '3') poleIndex = 2;
        else return;

        if (selectedSourcePole == -1) {
            // [Il tuo codice per selezionare il disco rimane invariato]
            if (poles[poleIndex].empty()) {
                Eng::Base::getInstance().setMessage("Empty pole!");
            }
            else {
                selectedSourcePole = poleIndex;
                int diskID = poles[poleIndex].back();
                Eng::Base::getInstance().setMessage(
                    "Selected disk " + std::to_string(diskID) + ". Choose destination pole."
                );
                updateVisuals();
            }
        }
        else {
            int source = selectedSourcePole;
            int dest = poleIndex;
            Eng::Base::getInstance().setMessage("");

            if (source == dest) {
                selectedSourcePole = -1;
                updateVisuals();
                return;
            }

            int diskToMove = poles[source].back();
            bool validMove = true;

            if (!poles[dest].empty()) {
                int topDiskDest = poles[dest].back();
                if (diskToMove < topDiskDest) {
                    validMove = false;
                }
            }

            if (validMove) {
                poles[source].pop_back();
                poles[dest].push_back(diskToMove);

                // --- MODIFICA QUI: Salva la mossa nella storia e svuota i Redo ---
                history.push_back({ source, dest });
                redoList.clear();
            }
            else {
                Eng::Base::getInstance().setMessage("Invalid move!");
            }

            selectedSourcePole = -1;
            updateVisuals();
        }
    }

    
};

//=================
// Global Game
//=================

HanoiGame game;

//=================
// Input Callback
//=================

/**
 * @brief Keyboard callback for camera control and game interaction.
 */
void keyboardCallback(unsigned char key, int x, int y) {
    Eng::Base& eng = Eng::Base::getInstance();
    game.handleInput(key);

    float dt = 1.0f;

    switch (tolower(key)) {

    case 'z':
        game.undoMove();
        break;

    case 'x':
        game.redoMove();
        break;
    case 'w': eng.moveCamera(eng::FORWARD, dt); break;
    case 's': eng.moveCamera(eng::BACKWARD, dt); break;
    case 'a': eng.moveCamera(eng::LEFT, dt); break;
    case 'd': eng.moveCamera(eng::RIGHT, dt); break;
    case 'q': eng.moveCamera(eng::UP, dt); break;
    case 'e': eng.moveCamera(eng::DOWN, dt); break;


    case 'k':
        eng.pitchCamera(-5.0f); // Guarda verso il basso
        break;

    case 'i':
        eng.pitchCamera(5.0f);  // Guarda verso l'alto
        break;

    case 'j':
        eng.rotateCamera(-5.0f);
        break;

    case 'l':

        eng.rotateCamera(5.0f);
        break;

    case 'o': {
        static bool isSideView = false;
        if (!isSideView) {
            eng.setCameraPosition(glm::vec3(60.0f, 10.0f, -40.0f), glm::vec3(0.0f, 1.0f, 0.0f), -180.0f, 0.0f);
        }
        else {
            eng.setCameraPosition(glm::vec3(0.0f, 10.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
        }
        isSideView = !isSideView;
        break;
    }


    case 'u': {
        static bool isTopView = false;
        if (!isTopView) {
            eng.setCameraPosition(glm::vec3(0.0f, 60.0f, -40.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -89.0f);
        }
        else {
            eng.setCameraPosition(glm::vec3(0.0f, 10.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
        }
        isTopView = !isTopView;
        break;
    }

    case 'r':
        game.reset();
        break;

    case 'v':
        eng.toggleCameraProjection();
        break;

    // --- NUOVI TASTI PER IL LOD ---
    case '+':
    case '=':
        eng.detail++;
        // Mettiamo un limite massimo (dal tuo log di 3ds max avevi fino a 4 o 5 LOD)
        if (eng.detail > 4) eng.detail = 4;
        break;

    case '-':
        eng.detail--;
        // Il LOD non può scendere sotto lo zero (0 = Qualità Massima)
        if (eng.detail < 0) eng.detail = 0;
        break;

  

    case 27:
        eng.free();
        exit(0);
    }
}

//=================
// Main
//=================

/**
 * @brief Application entry point.
 */
int main(int argc, char* argv[]) {
    std::cout << "Client application example, A. Peternier (C) SUPSI" << std::endl;

    Eng::Base& eng = Eng::Base::getInstance();
    eng.setKeyboardCallback(keyboardCallback);

    eng.init("My OpenGL window", 640, 480, argc, argv);


    //eng.addLight(10, 5, 0, 1, 0, 0, 1);
    eng.createOrb(0.0f, 5.0f, -47.0f);
    eng.createOrb(0.0f, 5.0f, -65.0f);

    eng.addText(eng::TextHUD("[ 1, 2, 3 [ Z, X]] : Select disk UNDO REDO", 10.0f, 20.0f, 1.0f, 1.0f, 0.0f));
    eng.addText(eng::TextHUD("[ J, L, I, K ] : Rotate Camera", 10.0f, 35.0f, 0.6f, 0.8f, 1.0f));
    eng.addText(eng::TextHUD("[ W, A, S, D ] : Move Camera", 10.0f, 50.0f, 0.6f, 0.8f, 1.0f));
    eng.addText(eng::TextHUD("[ U ] : Up view", 10.0f, 65.0f, 0.6f, 0.8f, 1.0f));
    eng.addText(eng::TextHUD("[ Q,E ] : UP,DOWN Move", 10.0f, 80.0f, 0.6f, 0.8f, 1.0f));
    eng.addText(eng::TextHUD("[ R, O ] : Reset Game/Camera", 10.0f, 95.0f, 1.0f, 0.5f, 0.0f));
    eng.addText(eng::TextHUD("[ +, - ] : Change LOD level", 10.0f, 110.0f, 1.0f, 1.0f, 0.0f));
    eng.addText(eng::TextHUD("[ V ] : Toggle Ortho/Perspective", 10.0f, 125.0f, 1.0f, 0.8f, 0.2f));
  
   
    //se maggiore ombra piu piccola
    eng.addShadow("Saber1", 0.0f, 500.0f, 0.0f);
    eng.addShadow("Table", 0.0f, 30.0f, 0.0f);

    game.init();
    eng.startLoop();

    eng.free();
    return 0;
}
