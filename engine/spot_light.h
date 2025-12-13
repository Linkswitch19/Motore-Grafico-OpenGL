#pragma once
#include "light.h"

namespace eng {
    class ENG_API SpotLight : public Light {
    private:
        glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
        float cutoff = 45.0f;
        float radius = 10.0f;
        float exponent = 1.0f;
    public:
        void set_direction(const glm::vec3& d);
        void set_cutoff(float c);
        void set_radius(float r);
        void set_exponent(float e);
    };
}
