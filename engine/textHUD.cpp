#include "textHUD.h"
#include <GL/freeglut.h> 


ENG_API eng::TextHUD::TextHUD(std::string content, float x, float y, float r, float g, float b)
    : content(content), x(x), y(y), r(r), g(g), b(b)
{
}


void ENG_API eng::TextHUD::setContent(std::string newContent) {
    this->content = newContent;
}


void ENG_API eng::TextHUD::render() {
  
    glColor3f(r, g, b);

   
    glRasterPos2f(x, y);

  
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)content.c_str());
}
