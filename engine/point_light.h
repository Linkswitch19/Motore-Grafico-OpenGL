#pragma once
#include "light.h"

namespace eng {
    class ENG_API PointLight : public Light {
    private:
        float radius = 10.0f;
    public:
        void set_radius(float r);
    };
}