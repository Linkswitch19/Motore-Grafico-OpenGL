/**
 * @file    point_light.h
 * @brief   Defines the PointLight class.
 *
 * This file contains the definition for a point light source, which models
 * an omni-directional light emitter that attenuates over distance.
 */

#pragma once
#include "light.h"

namespace eng {

    /**
     * @class PointLight
     * @brief Represents an omni-directional light source located at a single point in space.
     *
     * A point light emits light uniformly in all directions. Its brightness
     * typically decreases (attenuates) with the distance from the source.
     * Used to simulate lamps, candles, or small light bulbs.
     *
     */
    class ENG_API PointLight : public Light {
    private:
        /** @brief The effective radius or range of the light source.
         * Used to model light attenuation (how fast the light brightness drops off).
         */
        float radius = 10.0f;

    public:
        /**
         * @brief Sets the effective radius or attenuation range of the light.
         * @param r The new radius value.
         */
        void set_radius(float r);
        float get_radius() const;
    };
}