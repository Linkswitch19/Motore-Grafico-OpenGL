#pragma once
#include "light.h"

namespace eng {
    class ENG_API DirectionalLight : public Light {
    private:
        glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
    public:
        void set_direction(const glm::vec3& d);
    };
}