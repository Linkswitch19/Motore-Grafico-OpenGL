#include "texture.h"

namespace eng {

    ENG_API Texture::Texture(const std::string& path) : filePath(path) {}

    std::string ENG_API Texture::get_path() const {
        return filePath;
    }

}