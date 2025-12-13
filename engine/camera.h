/**
 * @file		camera.h
 * @brief	Classe Camera per la gestione della vista e del movimento
 */

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "lib.h"

 // Definizioni per il movimento della camera


namespace eng {
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // Valori di default per le impostazioni della camera
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 10.0f; // Velocità di movimento
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

    class ENG_API Camera
    {
    public:
        // Attributi Camera
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        // Angoli di Eulero
        float Yaw;
        float Pitch;

        // Opzioni Camera
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

        /**
         * @brief Costruttore con vettori
         */
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

        /**
         * @brief Costruttore con scalari
         */
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

        /**
         * @brief Restituisce la View Matrix calcolata (LookAt)
         */
        glm::mat4 GetViewMatrix();

        /**
         * @brief Elabora l'input da tastiera
         * @param direction Direzione del movimento (enum)
         * @param deltaTime Tempo trascorso dall'ultimo frame (per normalizzare la velocità)
         */
        void ProcessKeyboard(Camera_Movement direction, float deltaTime);

        /**
         * @brief Elabora l'input del mouse (per guardare in giro)
         * @param xoffset Spostamento mouse X
         * @param yoffset Spostamento mouse Y
         * @param constrainPitch Blocca la rotazione verticale per evitare il ribaltamento
         */
        void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    private:
        /**
         * @brief Calcola i vettori Front, Right e Up basandosi sugli angoli di Eulero
         */
        void updateCameraVectors();
    };
}