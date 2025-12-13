
/**
* @file		camera.cpp
* @brief	Implementazione della classe Camera
*/

#include "camera.h"
namespace eng {



 // Costruttore con vettori
    ENG_API Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Costruttore con scalari
    ENG_API Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Restituisce la View Matrix
    glm::mat4 ENG_API Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Gestione Tastiera (WASD)
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

        // Opzionale: Movimento verticale (Q/E o Spazio/Shift)
        if (direction == UP)
            Position += WorldUp * velocity;
        if (direction == DOWN)
            Position -= WorldUp * velocity;
    }

    // Gestione Mouse (Rotazione visuale)
    void ENG_API Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // Assicura che, quando il pitch è fuori dai limiti, lo schermo non si capovolga
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Aggiorna Front, Right e Up Vectors usando i nuovi angoli di Eulero
        updateCameraVectors();
    }

    // Calcoli matematici interni
    void ENG_API Camera::updateCameraVectors()
    {
        // Calcola il nuovo vettore Front
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // Ricalcola anche Right e Up vector
        // Normalizziamo i vettori, perché la loro lunghezza si avvicina a 0 più guardi in alto o in basso
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

}