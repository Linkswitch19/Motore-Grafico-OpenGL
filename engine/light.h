/**
 * @file    light.h
 * @brief   Defines the base Light class.
 *
 * This file contains the abstract base definition for light sources in the engine.
 * Since it inherits from Node, lights can be positioned and rotated within the scene graph.
 */

#pragma once
#include "node.h"
#include "lib.h"

namespace eng {

    /**
     * @class Light
     * @brief Abstract base class for all light sources in the scene graph.
     *
     * Inherits from Node to allow lights to be attached to other objects, moved,
     * and transformed using the scene hierarchy. It stores the fundamental color
     * properties common to all light types (Point, Directional, Spot).
     */
    class ENG_API Light : public Node {
    protected:
        /** * @brief The color of the light that creates diffuse illumination.
         * This is the main color of the light that illuminates objects evenly.
         */
        glm::vec3 diffuseColor{ 1.0f };

        /** * @brief The color of the light that creates specular highlights.
         * This defines the color of the "shiny spot" on glossy surfaces.
         */
        glm::vec3 specularColor{ 1.0f };

    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Light() = default;

        /**
         * @brief Sets the diffuse color of the light.
         *

[Image of diffuse reflection diagram]

         * @param c The RGB color vector (values typically between 0.0 and 1.0).
         */
        void set_diffuse_color(const glm::vec3& c);

        /**
         * @brief Sets the specular color of the light.
         *

[Image of specular reflection diagram]

         * @param c The RGB color vector (usually white or the light's color).
         */
        void set_specular_color(const glm::vec3& c);


        glm::vec3 get_diffuse_color() const;
        glm::vec3 get_specular_color() const;
    };

}