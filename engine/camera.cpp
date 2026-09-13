/**
* @file		camera.cpp
* @brief	Implementation of the Camera class.
* Handles camera movement, rotation, and view matrix calculation using Euler angles.
*/

#include "camera.h"

namespace eng {

    /**
     * @brief Constructor using vector values.
     * @param position Initial position of the camera.
     * @param up The up direction of the world (usually 0,1,0).
     * @param yaw Initial yaw angle (horizontal rotation).
     * @param pitch Initial pitch angle (vertical rotation).
     */
    ENG_API Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    /**
     * @brief Constructor using scalar values.
     * @param posX X coordinate of position.
     * @param posY Y coordinate of position.
     * @param posZ Z coordinate of position.
     * @param upX X coordinate of world up vector.
     * @param upY Y coordinate of world up vector.
     * @param upZ Z coordinate of world up vector.
     * @param yaw Initial yaw angle.
     * @param pitch Initial pitch angle.
     */
    ENG_API Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    /**
     * @brief Calculates and returns the view matrix.
     * Used by the shader or the fixed pipeline to transform world coordinates to view coordinates.
     * @return glm::mat4 The LookAt matrix.
     */
    glm::mat4 ENG_API Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    /**
     * @brief Processes input received from any keyboard-like input system.
     * Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems).
     * @param direction The direction of movement (FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN).
     * @param deltaTime Time elapsed between the current and last frame to ensure smooth movement.
     */
    void ENG_API Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += WorldUp * velocity;
        if (direction == DOWN)
            Position -= WorldUp * velocity;
    }

   

    /**
     * @brief Calculates the front vector from the Camera's (Euler) Angles.
     * Internal function to update the Right and Up vectors based on the new Front vector.
     */
    void ENG_API Camera::updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // Recalculate Right and Up vector
        // Normalize the vectors, because their length gets closer to 0 the more you look up or down
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    /**
     * @brief Manually rotates the camera yaw by a specific angle.
     * @param angle The angle in degrees to rotate the camera.
     */
    void ENG_API Camera::RotateBy(float angle)
    {
        Yaw += angle;

        // Keep Yaw within 0-360 range 
        if (Yaw > 360.0f) Yaw -= 360.0f;
        if (Yaw < -360.0f) Yaw += 360.0f;

        updateCameraVectors();
    }


    /**
     * @brief Restituisce la matrice di proiezione attuale (Prospettica o Ortografica)
     */
    glm::mat4 ENG_API Camera::GetProjectionMatrix()
    {
        if (isOrthographic) {
            // Matrice Ortografica 3D
            return glm::ortho(-OrthoScale * AspectRatio, OrthoScale * AspectRatio, -OrthoScale, OrthoScale, NearPlane, FarPlane);
        }
        else {
            // Matrice Prospettica classica
            return glm::perspective(glm::radians(Zoom), AspectRatio, NearPlane, FarPlane);
        }
    }

    /**
     * @brief Cambia la modalità di proiezione
     */
    void ENG_API Camera::ToggleProjection()
    {
        isOrthographic = !isOrthographic;
    }

    /**
     * @brief Aggiorna l'aspect ratio quando la finestra viene ridimensionata
     */
    void ENG_API Camera::SetAspectRatio(float width, float height)
    {
        if (height == 0.0f) height = 1.0f; // Evita divisioni per zero
        AspectRatio = width / height;
    }

    void ENG_API Camera::SetPositionAndOrientation(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors(); // Ricalcola dove sta guardando
    }

    /**
     * @brief Ruota la visuale della telecamera verso l'alto o verso il basso.
     * @param angle L'angolo in gradi.
     */
    void ENG_API Camera::PitchBy(float angle)
    {
        Pitch += angle;

        // Regola d'oro: Blocchiamo il pitch per evitare che la telecamera si capovolga!
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        updateCameraVectors(); // Ricalcola dove stiamo guardando
    }


}