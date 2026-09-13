#include "textHUD.h"
#include <GL/freeglut.h> 

/**
 * @brief Costruttore della classe TextHUD.
 * Inizializza un elemento di testo per l'interfaccia utente (HUD) con contenuto, posizione e colore.
 * * @param content La stringa di testo iniziale da visualizzare.
 * @param x La coordinata X dello schermo in cui posizionare il testo (solitamente in pixel o coordinate ortografiche).
 * @param y La coordinata Y dello schermo in cui posizionare il testo.
 * @param r La componente rossa del colore del testo (valori da 0.0 a 1.0).
 * @param g La componente verde del colore del testo (valori da 0.0 a 1.0).
 * @param b La componente blu del colore del testo (valori da 0.0 a 1.0).
 */
ENG_API eng::TextHUD::TextHUD(std::string content, float x, float y, float r, float g, float b)
    : content(content), x(x), y(y), r(r), g(g), b(b)
{
}

/**
 * @brief Aggiorna o modifica il testo visualizzato dall'elemento HUD.
 * * @param newContent La nuova stringa di testo da mostrare a schermo.
 */
void ENG_API eng::TextHUD::setContent(std::string newContent) {
    this->content = newContent;
}

/**
 * @brief Disegna fisicamente il testo sullo schermo.
 * * Imposta il colore corrente di OpenGL, posiziona il cursore raster (glRasterPos2f)
 * alle coordinate specificate e renderizza la stringa carattere per carattere
 * utilizzando il font bitmap predefinito di GLUT (GLUT_BITMAP_8_BY_13).
 */
void ENG_API eng::TextHUD::render() {
  
    glColor3f(r, g, b);

   
    glRasterPos2f(x, y);

  
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)content.c_str());
}
