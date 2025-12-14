/**
 * @file		main.cpp
 * @brief	Client application (that uses the graphics engine)
 *
 * @author	Achille Peternier (C) SUPSI [achille.peternier@supsi.ch] << change this to your group members
 */



//////////////
// #INCLUDE //
//////////////

   // Library header:
 #include "engine.h"

   // C/C++:
 #include <iostream>
 #include <vector>

#include <math.h>
#include "node.h"
#include "textHUD.h"
#include "enums.h"

#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
#define _CRT_SECURE_NO_WARNINGS


// Configurazione posizioni dei pali nel mondo 3D (da aggiustare in base al modello)
 float POLE_X_POSITIONS[3] = { -20.0f, 0.0f, 20.0f }; // Palo 1, 2, 3
 float DISK_HEIGHT = 0.5f;     // Altezza di uno step (spessore disco)
 float BASE_HEIGHT = 0.0f;     // Altezza base da terra


 class HanoiGame {
 private:
     // Stato logico: 3 pali, ognuno contiene gli ID dei dischi (1 a 7)
     std::vector<int> poles[3];

     // Riferimenti ai nodi grafici per aggiornarli
     std::shared_ptr<eng::Node> diskNodes[7];
     std::string poleNodeNames[3] = { "Palo1", "Palo2", "Palo3" };

     int selectedSourcePole = -1; // -1 significa "nessuna selezione", 0-2 è l'indice

 public:
     void init() {
         auto& engine = Eng::Base::getInstance();

         // 1. Carica i nodi
         for (int i = 0; i < 7; i++) {
             std::string name = "Disco" + std::to_string(i + 1);
             diskNodes[i] = engine.getNode(name);

             if (!diskNodes[i]) std::cout << "ERRORE: " << name << " non trovato" << std::endl;
         }

         // --- CALIBRAZIONE AUTOMATICA ---
         // Usiamo il disco più basso (Disco7, indice 6) per capire dove sta il Palo 1 e a che altezza.
         if (diskNodes[6]) {
             glm::mat4 m = diskNodes[6]->get_base_matrix();

             float startX = m[3][0]; // La X del disco nel file OVO
             float startY = m[3][1]; // La Y del disco nel file OVO

             std::cout << "Calibrazione: Disco7 trovato a X=" << startX << " Y=" << startY << std::endl;

             // Impostiamo le posizioni dei pali basandoci su questo
             // Assumiamo che la distanza tra i pali sia fissa, es. 40 unità
             // Oppure guarda nel tuo OVO quanto distano i pali.
             float distance = 40.0f; // <--- PROVA A CAMBIARE QUESTO SE I PALI SONO TROPPO VICINI/LONTANI

             POLE_X_POSITIONS[0] = startX;            // Palo Sinistra (dove sta il disco ora)
             POLE_X_POSITIONS[1] = startX + distance; // Palo Centro
             POLE_X_POSITIONS[2] = startX + (distance * 2); // Palo Destra

             BASE_HEIGHT = startY; // L'altezza base è quella del disco attuale
         }
         // -------------------------------

         for (int i = 1; i <= 7; i++) {
             poles[0].push_back(i);
         }

         std::cout << "--- HANOI GAME STARTED ---" << std::endl;
         updateVisuals();
     }


     void updateVisuals() {
         auto& engine = Eng::Base::getInstance();

         const float HOVER_HEIGHT = 20.0f;

         for (int p = 0; p < 3; p++) {
             float currentY = 0.0f; // Altezza base relativa al palo

             for (size_t i = 0; i < poles[p].size(); i++) {
                 int diskID = poles[p][i];
                 int diskIndex = diskID - 1;

                 std::string diskName = "Disco" + std::to_string(diskID);

                 // Usa i nomi dei tuoi pali (Palo1, Palo2, Palo3)
                 // Assicurati che l'array poleNodeNames sia definito nella classe o usa questa logica:
                 std::string poleName = "Palo" + std::to_string(p + 1);

                 // 1. Attacca al padre corretto
                 engine.setParent(diskName, poleName);

                 // 2. Calcola la posizione Y
                 float drawY = currentY;

                 // Se questo è il palo selezionato E questo è il disco in cima (l'ultimo)
                 if (p == selectedSourcePole && i == poles[p].size() - 1) {
                     drawY = HOVER_HEIGHT; 
                 }
                 // -------------------------------------------------

                 if (diskNodes[diskIndex]) {
                     glm::mat4 localMat = glm::mat4(1.0f);
                     localMat = glm::translate(localMat, glm::vec3(0.0f, drawY, 0.0f));
                     diskNodes[diskIndex]->set_base_matrix(localMat);
                 }

                 currentY += DISK_HEIGHT; // Incrementa per il prossimo disco nella pila
             }
         }
     }

     void handleInput(unsigned char key) {
         int poleIndex = -1;
         if (key == '1') poleIndex = 0;
         else if (key == '2') poleIndex = 1;
         else if (key == '3') poleIndex = 2;
         else return; // Tasto non valido per il gioco

         if (selectedSourcePole == -1) {
             // --- FASE 1: Selezione Sorgente ---
             if (poles[poleIndex].empty()) {
                 Eng::Base::getInstance().setMessage("Palo Vuoto!");
             }
             else {
                 selectedSourcePole = poleIndex;

                 int diskID = poles[poleIndex].back();


                 std::string msg = "Hai preso il Disco " + std::to_string(diskID) + " (Scegli dove metterlo)";
                 updateVisuals();


                 Eng::Base::getInstance().setMessage(msg);

             }
         }
         else {
             // --- FASE 2: Selezione Destinazione ---
             int source = selectedSourcePole;
             int dest = poleIndex;
             Eng::Base::getInstance().setMessage("");

             // Annulla selezione se si preme lo stesso palo
             if (source == dest) {
                 std::cout << "Selezione annullata." << std::endl;
                 selectedSourcePole = -1;
                 updateVisuals();
                 return;
             }

             // Logica Hanoi: Verifica validità mossa
             int diskToMove = poles[source].back(); // Il disco in cima al palo sorgente

             bool validMove = true;
             if (!poles[dest].empty()) {

                 int topDiskDest = poles[dest].back();
                 if (diskToMove < topDiskDest) { // <--- CAMBIATO DA > A <
                     validMove = false;
                     std::cout << "MOSSA INVALIDA: Non puoi mettere un disco GRANDE (ID "
                         << diskToMove << ") su uno PICCOLO (ID " << topDiskDest << ")!" << std::endl;
                 }


             }

             if (validMove) {
                 // Esegui sposta logico
                 poles[source].pop_back();
                 poles[dest].push_back(diskToMove);
                 std::cout << "Disco spostato da " << (source + 1) << " a " << (dest + 1) << std::endl;
             }

             // Resetta selezione
             selectedSourcePole = -1;
             updateVisuals();
         }
     }

 };




HanoiGame game;


void keyboardCallback(unsigned char key, int x, int y) {
    // Ottieni l'istanza dell'engine
    Eng::Base& eng = Eng::Base::getInstance();
    game.handleInput(key);

    float dt = 1.0f; // Delta time fittizio

    // Usa tolower per uniformare maiuscole/minuscole
    switch (tolower(key)) {

        // --- MOVIMENTO WASD ---
    case 'w': eng.moveCamera(eng::FORWARD, dt); break;
    case 's': eng.moveCamera(eng::BACKWARD, dt); break;
    case 'a': eng.moveCamera(eng::LEFT, dt); break;
    case 'd': eng.moveCamera(eng::RIGHT, dt); break;
    case 'q': eng.moveCamera(eng::UP, dt); break;   // Opzionale
    case 'e': eng.moveCamera(eng::DOWN, dt); break; // Opzionale

        // --- ROTAZIONE ---
    case 'k':
        eng.rotateCamera(-90.0f);
        std::cout << "Ruotato di 90 gradi" << std::endl;
        break;

        // --- VISUALE LATERALE (J) ---
    case 'j': {
        static bool isSideView = false;
        if (!isSideView) {
            // Vai di lato: Pos(60, 10, -40), Up(0,1,0), Yaw(-180), Pitch(0)
            eng.setCameraPosition(glm::vec3(60.0f, 10.0f, -40.0f), glm::vec3(0.0f, 1.0f, 0.0f), -180.0f, 0.0f);
            isSideView = true;
        }
        else {
            // Torna normale
            eng.setCameraPosition(glm::vec3(0.0f, 10.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
            isSideView = false;
        }
        break;
    }

            // --- VISTA DALL'ALTO (U) ---
    case 'u': {
        static bool isTopView = false;
        if (!isTopView) {
            // Vai sopra: Pos(0, 60, -40), Yaw(-90), Pitch(-89)
            eng.setCameraPosition(glm::vec3(0.0f, 60.0f, -40.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -89.0f);
            std::cout << "Visuale: ALTO" << std::endl;
            isTopView = true;
        }
        else {
            // Reset normale
            eng.setCameraPosition(glm::vec3(0.0f, 10.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
            std::cout << "Visuale: NORMALE" << std::endl;
            isTopView = false;
        }
        break;
    }

            // ESC per uscire
    case 27:
        eng.free();
        exit(0);
        break;
    };
    
 




}



//////////
// MAIN //
//////////

/**
 * Application entry point.
 * @param argc number of command-line arguments passed
 * @param argv array containing up to argc passed arguments
 * @return error code (0 on success, error code otherwise)
 */
int main(int argc, char *argv[])
{
   // Credits:
   std::cout << "Client application example, A. Peternier (C) SUPSI" << std::endl;
   std::cout << std::endl;

   // Init engine:
   Eng::Base &eng = Eng::Base::getInstance();
 
   eng.setKeyboardCallback(keyboardCallback);
   
   eng.init("My OpenGL window", 640, 480,argc,argv);
   eng.loadTexture("Wood094_1K-PNG_Color.dds", "Base");
   eng.loadTexture("Wood094_1K-PNG_Color.dds", "Palo1");
   eng.loadTexture("Wood094_1K-PNG_Color.dds", "Palo2");
   eng.loadTexture("Wood094_1K-PNG_Color.dds", "Palo3");

   eng.createOrb(0.0f, 5.0f, -47.0f);
   eng.createOrb(0.0f, 5.0f, -65.0f);
   
   
   eng.addText(eng::TextHUD("[ 1, 2, 3 ] : SELEZIONA DISCO", 10.0f, 20.0f, 1.0f, 1.0f, 0.0f));
   eng::TextHUD istruzioniData("[ J ] : Vista Laterale", 10.0f, 35.0f, 0.6f, 0.8f, 1.0f);
   eng.addText(istruzioniData);
   eng.addText(eng::TextHUD("[ W, A, S, D ] : Camera", 10.0f, 50.0f, 0.6f, 0.8f, 1.0f));
   game.init();
   
   eng.startLoop();
   

 
   

   // Release engine:
   eng.free();

   // Done:
   std::cout << "\n[application terminated]" << std::endl;
   return 0;
}








