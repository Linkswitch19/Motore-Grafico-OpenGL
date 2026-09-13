#include "hud_manager.h"
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>

namespace eng {

    /**
     * @brief Aggiunge un oggetto di testo personalizzato alla lista dell'HUD.
     * I testi aggiunti con questo metodo verranno renderizzati ad ogni fotogramma
     * fino a quando l'HUD non verrà pulito.
     * @param textObject L'istanza di TextHUD contenente testo, colore e coordinate.
     */
    void HUDManager::addText(const TextHUD& textObject) {
        hudList.push_back(textObject);
    }

    /**
     * @brief Imposta un messaggio di sistema in evidenza.
     * Questo messaggio viene tipicamente mostrato al centro dello schermo (es. "Invalid move!")
     * ed è separato dai normali testi di overlay.
     * @param msg La stringa contenente il messaggio da visualizzare.
     */
    void HUDManager::setMessage(const std::string& msg) {
        currentMessage = msg;
    }

    /**
     * @brief Svuota la lista dei testi personalizzati dell'HUD.
     */
    void HUDManager::clearHUD() {
        hudList.clear();
    }

    /**
     * @brief Renderizza l'intera interfaccia 2D (HUD) sopra la scena 3D.
     * @details Imposta temporaneamente una proiezione ortografica basata sulle
     * dimensioni attuali della finestra, disabilita l'illuminazione e le texture 3D
     * per disegnare i testi ("piatti" sullo schermo). Stampa di default gli FPS
     * e il livello di LOD, seguiti dai testi personalizzati e dall'eventuale
     * messaggio di sistema, per poi ripristinare le luci.
     * * @param fps I fotogrammi per secondo attuali da mostrare in alto a destra.
     * @param detail Il livello di dettaglio (LOD) attuale da mostrare sotto gli FPS.
     * @param windowWidth La larghezza corrente della finestra (in pixel).
     * @param windowHeight L'altezza corrente della finestra (in pixel).
     */
    void HUDManager::renderHUD(int fps, int detail, int windowWidth, int windowHeight) {
        // Calcola la matrice ortogonale in base alla finestra attuale
        glm::mat4 ortho = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(ortho));
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        glColor3f(1.0f, 1.0f, 1.0f);

        char buffer[128];
        float textX = windowWidth - 90.0f;
        float textY = windowHeight - 50.0f;
        float stepY = 15.0f;

        snprintf(buffer, sizeof(buffer), "FPS: %d", fps);
        glRasterPos2f(textX, textY);
        glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);

        textY += stepY;
        snprintf(buffer, sizeof(buffer), "LOD: %d", detail);
        glRasterPos2f(textX, textY);
        glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);

        for (auto& textItem : hudList) {
            textItem.render();
        }

        if (!currentMessage.empty()) {
            glColor3f(1.0f, 1.0f, 0.0f);
            glRasterPos2f(windowWidth / 2.0f - 50.0f, windowHeight - 50.0f);
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)currentMessage.c_str());
        }

        glEnable(GL_LIGHTING);
    }

}