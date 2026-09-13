/**
 * @file    lib.h
 * @brief   Shared definitions and helper types.
 *
 * This file contains common enumerations, macros, and structures used
 * across the entire engine to ensure consistency (e.g., input abstraction).
 */

#pragma once

namespace eng {

    /**
     * @enum Camera_Movement
     * @brief Defines the possible abstract directions for camera navigation.
     *
     * This enum is used to decouple the specific input keys (W, A, S, D)
     * from the camera logic. The camera processes these abstract directions
     * to update its position vector.
     *
     */
    enum Camera_Movement {
        FORWARD,  ///< Move forward along the camera's front vector.
        BACKWARD, ///< Move backward (opposite to front vector).
        LEFT,     ///< Strafe left (along the negative right vector).
        RIGHT,    ///< Strafe right (along the positive right vector).
        UP,       ///< Move vertically up (usually along World Up).
        DOWN      ///< Move vertically down (opposite to World Up).
    };

}