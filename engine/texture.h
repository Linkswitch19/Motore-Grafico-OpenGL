#pragma once
#pragma once
#include <string>
#include "lib.h"

namespace eng {

    class ENG_API Texture {
    private:
        std::string filePath;
    public:
        Texture(const std::string& path);
        virtual ~Texture() = default;
        std::string get_path() const;
    };

} 