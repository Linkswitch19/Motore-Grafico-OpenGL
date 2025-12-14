#pragma once
#include <string>
#include <GL/freeglut.h> 
#include <FreeImage.h> // <--- Includi l'header di FreeImage
#include "lib.h"

namespace eng {

    class ENG_API Texture {
    private:
        std::string filePath;
        GLuint textureId;
        unsigned int width;
        unsigned int height;

    public:
        Texture(const std::string& path);
        virtual ~Texture();

        std::string get_path() const;
        GLuint get_texture_id() const;
        unsigned int get_width() const { return width; }
        unsigned int get_height() const { return height; }
    };

}