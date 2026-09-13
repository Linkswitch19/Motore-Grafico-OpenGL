/**
 * @file    directional_light.h
 * @brief   Defines the DirectionalLight class.
 *
 * This file contains the definition for a directional light source, which mimics
 * global illumination sources like the sun.
 */

#pragma once
#include "light.h"

namespace eng {

    /**
     * @class DirectionalLight
     * @brief Represents an infinite light source with parallel rays.
     *
     * A directional light creates light rays that are parallel to each other.
     * It is typically used to simulate the sun. Unlike Point or Spot lights,
     * its position does not affect lighting, only its direction does.
     * *
     */
    class ENG_API DirectionalLight : public Light {
    private:
        /** * @brief The direction vector the light travels in.
         * Default is pointing straight down (0, -1, 0).
         */
        glm::vec3 direction{ 0.0f, -1.0f, 0.0f };

    public:
        /**
         * @brief Sets the direction of the light rays.
         * @param d The direction vector (should usually be normalized).
         */
        void set_direction(const glm::vec3& d);

        glm::vec3 get_direction() const;
    };
}