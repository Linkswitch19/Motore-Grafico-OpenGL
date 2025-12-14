#include "texture.h"
#include <iostream>

using namespace eng;

ENG_API Texture::Texture(const std::string& path) : filePath(path), textureId(0), width(0), height(0) {

    // 1. Determina il formato del file
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path.c_str(), 0);
    if (format == FIF_UNKNOWN) {
        format = FreeImage_GetFIFFromFilename(path.c_str());
    }
    if (format == FIF_UNKNOWN) {
        std::cout << "ERRORE: Formato immagine sconosciuto: " << path << std::endl;
        return;
    }

    // 2. Carica l'immagine in memoria
    FIBITMAP* bitmap = FreeImage_Load(format, path.c_str());
    if (!bitmap) {
        std::cout << "ERRORE: Impossibile caricare il file: " << path << std::endl;
        return;
    }

    // 3. Converti in 32 bit (RGBA) per massima compatibilità
    // FreeImage carica spesso in BGR, convertendo a 32 bit uniformiamo tutto.
    FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);

    // Scarichiamo l'originale, teniamo solo quello a 32 bit
    FreeImage_Unload(bitmap);
    FreeImage_FlipVertical(bitmap32);

    this->width = FreeImage_GetWidth(bitmap32);
    this->height = FreeImage_GetHeight(bitmap32);

    // Ottieni i puntatori ai dati grezzi
    BYTE* data = FreeImage_GetBits(bitmap32);

    // 4. Genera la Texture OpenGL
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Setup parametri texture (ripetizione e filtro lineare)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // 5. Carica i dati nella scheda video
    // Nota: FreeImage usa l'ordine BGRA, quindi usiamo GL_BGRA
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

    // Alternativa senza mipmap (meno bella da vedere da lontano):
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

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