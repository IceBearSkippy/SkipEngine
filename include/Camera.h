#pragma once
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 6.0f;
const float SENSITIVTY = 0.25f;
const float ZOOM = 45.0f;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

namespace Skip {

    // An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices
    class Camera
    {
    public:
        Camera();
        // Constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

        // Constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

        ~Camera();

        glm::mat4 GetViewMatrix();
        void ProcessKeyboard(Camera_Movement direction, float deltaTime);

        // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yOffset);

        float GetZoom();

        glm::vec3 GetPosition();

        glm::vec3 GetFront();

        glm::vec3 GetUp();
    private:
        // Camera Attributes
        glm::vec3 _position;
        glm::vec3 _front;
        glm::vec3 _up;
        glm::vec3 _right;
        glm::vec3 _worldUp;

        // Eular Angles
        float _yaw;
        float _pitch;

        // Camera options
        float _movementSpeed;
        float _mouseSensitivity;
        float _zoom;
        
        // Calculates the front vector from the Camera's (updated) Eular Angles
        void updateCameraVectors();
    };
}