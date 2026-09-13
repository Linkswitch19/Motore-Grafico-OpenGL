/**
 * @file    textHUD.h
 * @brief   Defines the TextHUD class for 2D text rendering.
 *
 * This file contains the definition of a helper class used to manage and
 * render individual text elements (like scores, FPS, or messages) on the
 * Heads-Up Display (HUD) overlay.
 */

#pragma once
#include <string>
#include <iostream>
#include "lib.h"

namespace eng {

    /**
     * @class TextHUD
     * @brief Manages the content, position, and color of a single 2D text element.
     *
     * Text elements are rendered in screen space (using an orthographic projection)
     * on top of the 3D scene.
     */
    class ENG_API TextHUD
    {
    private:
        /** @brief The actual string content to be displayed. */
        std::string content;

        /** @brief X screen coordinate (e.g., pixel coordinate). */
        float x, y;

        /** @brief RGB color components of the text. */
        float r, g, b;

    public:
        /**
         * @brief Constructor for the TextHUD element.
         * * @param content The initial string content.
         * @param x The X position in screen coordinates.
         * @param y The Y position in screen coordinates.
         * @param r Red color component (default 1.0f).
         * @param g Green color component (default 1.0f).
         * @param b Blue color component (default 1.0f).
         */
        TextHUD(std::string content, float x, float y, float r = 1.0f, float g = 1.0f, float b = 1.0f);

        /**
         * @brief Updates the string content of the HUD element.
         * @note Useful for dynamic values like scores, health, or FPS.
         * @param newContent The new string content.
         */
        void setContent(std::string newContent);

        /**
         * @brief Renders the text element to the screen using the current OpenGL context.
         * @note This method typically requires the GL projection matrix to be set to Orthographic.
         */
        void render();
    };
}


