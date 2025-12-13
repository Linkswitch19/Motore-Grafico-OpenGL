#include "spot_light.h"
namespace eng {
    void ENG_API SpotLight::set_direction(const glm::vec3& d) { direction = d; }
    void ENG_API SpotLight::set_cutoff(float c) { cutoff = c; }
    void ENG_API SpotLight::set_radius(float r) { radius = r; }
    void ENG_API SpotLight::set_exponent(float e) { exponent = e; }
}