#include "light_manager.h"
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>

namespace eng {

    /**
     * @brief Crea una sfera luminosa dinamica (orb) con luce verde predefinita.
     * Gestisce in automatico l'assegnazione dell'ID OpenGL (da GL_LIGHT0 in poi, fino a un massimo di 8).
     * @param x Posizione X iniziale dell'orb.
     * @param y Posizione Y iniziale dell'orb.
     * @param z Posizione Z iniziale dell'orb.
     */
    void LightManager::createOrb(float x, float y, float z) {
        if (activeLightCount >= 8) {
            std::cout << "[WARNING] Max light count reached (8)." << std::endl;
            return;
        }

        int lightID = GL_LIGHT0 + activeLightCount;
        activeLightCount++;
        orbsList.push_back({ glm::vec3(x, y, z), lightID });

        GLfloat Color[] = { 0.0f, 1.0f, 0.0f, 1.0f };
        GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };

        glEnable(lightID);
        glLightfv(lightID, GL_DIFFUSE, Color);
        glLightfv(lightID, GL_SPECULAR, Color);
        glLightfv(lightID, GL_AMBIENT, ambient);

        glLightf(lightID, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(lightID, GL_LINEAR_ATTENUATION, 0.1f);
        glLightf(lightID, GL_QUADRATIC_ATTENUATION, 0.02f);
    }

    /**
     * @brief Aggiunge una luce posizionale generica e statica alla scena.
     * @param x Coordinata X della luce.
     * @param y Coordinata Y della luce.
     * @param z Coordinata Z della luce.
     * @param r Componente rossa della luce (0.0 - 1.0).
     * @param g Componente verde della luce (0.0 - 1.0).
     * @param b Componente blu della luce (0.0 - 1.0).
     * @param a Componente alfa della luce (0.0 - 1.0).
     */
    void LightManager::addLight(float x, float y, float z, float r, float g, float b, float a) {
        if (activeLightCount >= 8) return;

        int lightID = GL_LIGHT0 + activeLightCount;
        activeLightCount++;

        GLfloat position[] = { x, y, z, 1.0f };
        GLfloat color[] = { r, g, b, a };
        GLfloat ambient[] = { r * 0.2f, g * 0.2f, b * 0.2f, a };

        glEnable(lightID);
        glLightfv(lightID, GL_POSITION, position);
        glLightfv(lightID, GL_DIFFUSE, color);
        glLightfv(lightID, GL_SPECULAR, color);
        glLightfv(lightID, GL_AMBIENT, ambient);

        glLightf(lightID, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(lightID, GL_LINEAR_ATTENUATION, 0.05f);
        glLightf(lightID, GL_QUADRATIC_ATTENUATION, 0.0f);
    }



    /**
     * @brief Registra una richiesta per proiettare un'ombra partendo da un nodo specifico.
     * L'ombra verrà effettivamente disegnata dal Renderer utilizzando queste informazioni.
     * @param nodeName Il nome del nodo (es. un oggetto 3D) che proietterà l'ombra.
     * @param x Posizione X della sorgente luminosa che causa l'ombra.
     * @param y Posizione Y della sorgente luminosa che causa l'ombra.
     * @param z Posizione Z della sorgente luminosa che causa l'ombra.
     */
    void LightManager::addShadow(const std::string& nodeName, float x, float y, float z) {

        shadowList.push_back({ nodeName, glm::vec3(x, y, z) });
    }

    /**
     * @brief Disegna fisicamente e aggiorna la logica delle sfere luminose (orbs).
     * Si occupa di disabilitare le texture, impostare un materiale "emissivo" per
     * far sembrare che l'oggetto brilli (effetto neon), calcolare un'oscillazione
     * verticale tramite la funzione seno, disegnare la sfera e aggiornare
     * la posizione della luce OpenGL.
     * @param rotationAngleY Angolo di rotazione della scena/orbita.
     * @param totalTime Tempo globale utilizzato per animare l'oscillazione sinusoidale sull'asse Y.
     */
    void LightManager::renderOrbs(float rotationAngleY, float totalTime) {
        glDisable(GL_TEXTURE_2D);
        if (orbsList.empty()) return;

        GLfloat neon[] = { 0.0f, 0.5f, 0.2f, 1.0f };
        GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, neon);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, neon);

        for (const auto& orb : orbsList) {
            float verticalOffset = sin(totalTime * 2.0f) * 3.0f;
            glPushMatrix();
            glTranslatef(orb.position.x, orb.position.y + verticalOffset, orb.position.z);
            glRotatef(rotationAngleY, 0.0f, 1.0f, 0.0f);

            GLfloat lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glLightfv(orb.lightID, GL_POSITION, lightPos);
            glutSolidSphere(2.0, 32, 32);
            glPopMatrix();
        }
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
    }

    /**
     * @brief Restituisce il numero totale di orbs attualmente creati.
     * @return int Quantità di orbs nella scena.
     */
    int LightManager::getOrbCount() const { return (int)orbsList.size(); }

    /**
     * @brief Restituisce in sola lettura la lista delle richieste per le ombre.
     * @return const std::vector<ShadowRequest>& Reference alla lista delle ombre.
     */
    const std::vector<ShadowRequest>& LightManager::getShadowRequests() const { return shadowList; }

}