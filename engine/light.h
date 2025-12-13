#pragma once
#include "node.h"
#include "lib.h"

namespace eng {

    class ENG_API Light : public Node {
    protected:
        glm::vec3 diffuseColor{ 1.0f };
        glm::vec3 specularColor{ 1.0f };
    public:
        virtual ~Light() = default;
        void set_diffuse_color(const glm::vec3& c);
        void set_specular_color(const glm::vec3& c);
    };

}