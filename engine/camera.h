/**
 * @file    camera.h
 * @brief   Defines the Camera class for 3D navigation.
 *
 * This file contains the Camera class which handles the calculation of the View Matrix,
 * processing of keyboard/mouse input, and management of Euler angles (Yaw, Pitch).
 */

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "lib.h"

namespace eng {

    // Default camera values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 10.0f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

    /**
     * @class Camera
     * @brief A camera class that processes input and calculates the corresponding Euler Angles, Vectors, and Matrices.
     * * The camera system is based on the Fly-cam style (First Person Shooter),
     * using Euler angles to determine direction.
     */
    class ENG_API Camera
    {
    public:
        // --- Camera Attributes ---

        
        /** @brief Camera position in World Space. */
        glm::vec3 Position;

        /** @brief The direction vector the camera is currently facing. */
        glm::vec3 Front;

        /** @brief The up vector relative to the camera's orientation. */
        glm::vec3 Up;

        /** @brief The right vector relative to the camera's orientation (perpendicular to Front and Up). */
        glm::vec3 Right;

        /** @brief The global up direction (usually 0,1,0). */
        glm::vec3 WorldUp;

     
        /** @brief Yaw angle (rotation around the vertical axis) in degrees. */
        float Yaw;

        /** @brief Pitch angle (rotation around the side axis) in degrees. */
        float Pitch;

        // --- Camera Options ---

        /** @brief Movement speed/velocity. */
        float MovementSpeed;

        /** @brief Mouse sensitivity multiplier for rotation. */
        float MouseSensitivity;

        /** @brief Field of View (Zoom level) in degrees. */
        float Zoom;

      
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

       
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

      
        glm::mat4 GetViewMatrix();

       
        void RotateBy(float angle);

        
        void ProcessKeyboard(Camera_Movement direction, float deltaTime);

        glm::mat4 GetProjectionMatrix();
        void ToggleProjection();
        void SetAspectRatio(float width, float height);
        void SetPositionAndOrientation(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
        void PitchBy(float angle);

     
        

    private:
        bool isOrthographic = false;
        float AspectRatio = 1.33f; // Default 4:3, verrà sovrascritto dal reshape
        float OrthoScale = 30.0f;  // Lo zoom in modalità ortografica
        float NearPlane = 1.0f;
        float FarPlane = 500.0f;

        void updateCameraVectors();
    };
}