#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "lib.h"
#include "texture.h"

namespace eng {

    class ENG_API Material {
    private:
        std::string name;
        glm::vec3 emission{ 0.0f };
        glm::vec3 ambient{ 0.0f };
        glm::vec3 diffuse{ 1.0f };
        glm::vec3 specular{ 0.0f };
        float shininess = 0.0f;
        std::shared_ptr<Texture> texture;

    public:
        Material() = default;
        virtual ~Material() = default;

        void set_name(const std::string& n);
        std::string get_name() const;

        void set_emission_color(const glm::vec3& c);
        void set_ambient_color(const glm::vec3& c);
        void set_diffuse_color(const glm::vec3& c);
        void set_specular_color(const glm::vec3& c);
        void set_shininess(float s);
        void set_texture(std::shared_ptr<Texture> tex);


        glm::vec3 get_emission_color() const;
        glm::vec3 get_ambient_color() const;
        glm::vec3 get_diffuse_color() const;
        glm::vec3 get_specular_color() const;
        float get_shininess() const;
        std::shared_ptr<Texture> get_texture() const;
    };

} 