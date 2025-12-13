#include "material.h"

namespace eng {

	void ENG_API Material::set_name(const std::string& n) { name = n; }
	std::string ENG_API Material::get_name() const { return name; }
	void ENG_API Material::set_emission_color(const glm::vec3& c) { emission = c; }
	void ENG_API Material::set_ambient_color(const glm::vec3& c) { ambient = c; }
	void ENG_API Material::set_diffuse_color(const glm::vec3& c) { diffuse = c; }
	void ENG_API Material::set_specular_color(const glm::vec3& c) { specular = c; }
	void ENG_API Material::set_shininess(float s) { shininess = s; }
	void ENG_API Material::set_texture(std::shared_ptr<Texture> tex) { texture = tex; }

}
