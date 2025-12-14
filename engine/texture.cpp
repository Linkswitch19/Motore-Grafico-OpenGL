#include "texture.h"
#include <iostream>

using namespace eng;

ENG_API Texture::Texture(const std::string& path) : filePath(path), textureId(0), width(0), height(0) {

    //formato file
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path.c_str(), 0);
    if (format == FIF_UNKNOWN) {
        format = FreeImage_GetFIFFromFilename(path.c_str());
    }
    if (format == FIF_UNKNOWN) {
        std::cout << "ERRORE: Formato immagine sconosciuto: " << path << std::endl;
        return;
    }

    //Load img
    FIBITMAP* bitmap = FreeImage_Load(format, path.c_str());
    if (!bitmap) {
        std::cout << "ERRORE: Impossibile caricare il file: " << path << std::endl;
        return;
    }

   
    FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);

   
    FreeImage_Unload(bitmap);
    FreeImage_FlipVertical(bitmap32);

    this->width = FreeImage_GetWidth(bitmap32);
    this->height = FreeImage_GetHeight(bitmap32);

  
    BYTE* data = FreeImage_GetBits(bitmap32);

    //Genera la Texture OpenGL
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

   //carica dati gpu
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

  

    std::cout << "Texture caricata con FreeImage (ID: " << textureId << "): " << path << std::endl;

    // 6. Pulizia memoria RAM
    FreeImage_Unload(bitmap32);
}

Texture::~Texture() {
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);
    }
}

std::string ENG_API Texture::get_path() const {
    return filePath;
}

GLuint ENG_API Texture::get_texture_id() const {
    return textureId;
}