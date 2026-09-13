/**
 * @file    texture.cpp
 * @brief   Implementation of the Texture class.
 *
 * Handles loading images from disk using FreeImage and converting them
 * into OpenGL texture objects with mipmapping support.
 */

#include "texture.h"
#include <iostream>
#include <FreeImage.h> 

using namespace eng;

/**
 * @brief Constructor. Loads an image file and generates an OpenGL texture.
 *
 * This function performs the following steps:
 * 1. Detects the image file format (PNG, JPG, BMP, etc.).
 * 2. Loads the image into RAM using FreeImage.
 * 3. Converts the image to 32-bit RGBA format for consistency.
 * 4. Flips the image vertically (necessary because OpenGL origin is bottom-left).
 * 5. Uploads the data to the GPU and generates Mipmaps. Little version of texture
 * 6. Frees the RAM used by FreeImage.
 *
 * @param path The file path to the image asset.
 */
ENG_API Texture::Texture(const std::string& path) : filePath(path), textureId(0), width(0), height(0) {

    // 1. Determine file format
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path.c_str(), 0);
    if (format == FIF_UNKNOWN) {
        format = FreeImage_GetFIFFromFilename(path.c_str());
    }
    if (format == FIF_UNKNOWN) {
        std::cout << "[ERROR] Unknown image format: " << path << std::endl;
        return;
    }

    // 2. Load image into memory
    FIBITMAP* bitmap = FreeImage_Load(format, path.c_str());
    if (!bitmap) {
        std::cout << "[ERROR] Failed to load file: " << path << std::endl;
        return;
    }

    // 3. Convert to 32-bit (RGBA) to ensure alpha channel presence
    FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
    FreeImage_Unload(bitmap); // Unload the original non-32bit version

    // 4. Flip vertically for OpenGL coordinate system
    FreeImage_FlipVertical(bitmap32);

    this->width = FreeImage_GetWidth(bitmap32);
    this->height = FreeImage_GetHeight(bitmap32);
    BYTE* data = FreeImage_GetBits(bitmap32);

    // 5. Generate OpenGL Texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Texture Parameters (Repeating and Linear Filtering)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Upload to GPU and Build Mipmaps
    // Using gluBuild2DMipmaps for automatic mipmap generation supports non-power-of-two images well
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

    std::cout << "[INFO] Texture loaded (ID: " << textureId << "): " << path << std::endl;

    // 6. Cleanup RAM
    FreeImage_Unload(bitmap32);
}

/**
 * @brief Destructor.
 * Deletes the texture from GPU memory to prevent leaks.
 */
Texture::~Texture() {
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);
    }
}

/**
 * @brief Gets the original file path of the texture.
 * @return The file path string.
 */
std::string ENG_API Texture::get_path() const {
    return filePath;
}

/**
 * @brief Gets the OpenGL ID of the texture.
 * @return The GLuint handle for the texture.
 */
GLuint ENG_API Texture::get_texture_id() const {
    return textureId;
}