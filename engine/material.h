/**
 * @file    material.h
 * @brief   Defines the Material class.
 *
 * This file contains the definition of the Material class, which encapsulates
 * all the surface properties required for lighting calculations and texturing
 * (following the Phong/Blinn-Phong illumination model).
 */

#pragma once
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "lib.h"
#include "texture.h"

namespace eng {

    /**
     * @class Material
     * @brief Defines the visual properties of a 3D surface.
     *
     * A material object holds color coefficients, shininess factor, and a
     * texture reference, which determine how the surface reflects light.
     *
     */
    class ENG_API Material {
    private:
        /** @brief Unique identifier or human-readable name for the material. */
        std::string name;

        /** @brief Emissive color: light emitted by the material itself, regardless of scene lights. */
        glm::vec3 emission{ 0.0f };

        /** @brief Ambient color: light reflected from uniform ambient light in the scene. */
        glm::vec3 ambient{ 0.0f };

        /** @brief Diffuse color: light reflected from directional sources (the main color of the surface). */
        glm::vec3 diffuse{ 1.0f };

        /** @brief Specular color: color used for shiny, directional highlights. */
        glm::vec3 specular{ 0.0f };

        /** @brief Shininess coefficient: controls the size/sharpness of the specular highlight. */
        float shininess = 0.0f;

        /** @brief Optional texture map applied to the surface. */
        std::shared_ptr<Texture> texture;

    public:
        /** @brief Default constructor. */
        Material() = default;

        /** @brief Virtual destructor. */
        virtual ~Material() = default;

        // --- Setter Methods ---

        /**
         * @brief Sets the material's name.
         * @param n The new name.
         */
        void set_name(const std::string& n);

        /**
         * @brief Sets the emissive color.
         * @param c The RGB color vector.
         */
        void set_emission_color(const glm::vec3& c);

        /**
         * @brief Sets the ambient color coefficient.
         * @param c The RGB color vector.
         */
        void set_ambient_color(const glm::vec3& c);

        /**
         * @brief Sets the diffuse color coefficient.
         * @param c The RGB color vector.
         */
        void set_diffuse_color(const glm::vec3& c);

        /**
         * @brief Sets the specular color coefficient.
         * @param c The RGB color vector.
         */
        void set_specular_color(const glm::vec3& c);

        /**
         * @brief Sets the shininess coefficient.
         * * Higher values result in sharper, smaller specular highlights.
         * @param s The shininess value (typically 0.0 to 128.0+).
         */
        void set_shininess(float s);

        /**
         * @brief Assigns a texture map to the material.
         * @param tex Shared pointer to the Texture object.
         */
        void set_texture(std::shared_ptr<Texture> tex);


        // --- Getter Methods ---

        /**
         * @brief Gets the material's name.
         * @return The name string.
         */
        std::string get_name() const;

        /**
         * @brief Gets the emissive color.
         * @return The RGB color vector.
         */
        glm::vec3 get_emission_color() const;

        /**
         * @brief Gets the ambient color coefficient.
         * @return The RGB color vector.
         */
        glm::vec3 get_ambient_color() const;

        /**
         * @brief Gets the diffuse color coefficient.
         * @return The RGB color vector.
         */
        glm::vec3 get_diffuse_color() const;

        /**
         * @brief Gets the specular color coefficient.
         * @return The RGB color vector.
         */
        glm::vec3 get_specular_color() const;

        /**
         * @brief Gets the shininess coefficient.
         * @return The shininess value.
         */
        float get_shininess() const;

        /**
         * @brief Gets the assigned texture.
         * @return Shared pointer to the Texture object.
         */
        std::shared_ptr<Texture> get_texture() const;
    };

}