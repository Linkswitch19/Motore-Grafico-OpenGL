/**
 * @file    spot_light.h
 * @brief   Defines the SpotLight class.
 *
 * This file contains the definition for a spot light source, which models
 * a directed light cone (like a flashlight or theatrical spotlight).
 */

#pragma once
#include "light.h"

namespace eng {

    /**
     * @class SpotLight
     * @brief Represents a directed light source with a conical shape.
     *
     * A spot light combines the position-dependent attenuation of a Point Light
     * with a defined direction, creating a focused cone of illumination.
     * Used to simulate flashlights or light beams.
     *
     */
    class ENG_API SpotLight : public Light {
    private:
        /** @brief The direction vector the light cone is pointing towards. */
        glm::vec3 direction{ 0.0f, -1.0f, 0.0f };

        /** @brief The cutoff angle (in degrees) that defines the size of the inner light cone.
         * Light is zero outside this angle.
         */
        float cutoff = 45.0f;

        /** @brief The effective radius or attenuation range of the light. */
        float radius = 10.0f;

        /** @brief The spot exponent, which controls the falloff or fuzziness of the cone's edge.
         * Higher values result in a harder, sharper edge.
         */
        float exponent = 1.0f;

    public:
        /**
         * @brief Sets the direction vector of the light beam.
         * @param d The direction vector (should be normalized).
         */
        void set_direction(const glm::vec3& d);

        /**
         * @brief Sets the cutoff angle, defining the width of the light cone.
         * @param c The cutoff angle in degrees.
         */
        void set_cutoff(float c);

        /**
         * @brief Sets the effective radius or attenuation range.
         * @param r The new radius value.
         */
        void set_radius(float r);

        /**
         * @brief Sets the spot exponent, controlling the edge falloff.
         * @param e The exponent value (1.0 or higher).
         */
        void set_exponent(float e);


        glm::vec3 get_direction() const;
        float get_cutoff() const;
        float get_radius() const;
        float get_exponent() const;
    };
}
