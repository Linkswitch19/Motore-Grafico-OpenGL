#include "light.h"
namespace eng {
    void ENG_API Light::set_diffuse_color(const glm::vec3& c) { diffuseColor = c; }
    void ENG_API Light::set_specular_color(const glm::vec3& c) { specularColor = c; }
}