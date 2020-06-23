#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Skip {
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Default camera values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 6.0f;
    const float SENSITIVTY = 0.25f;
    const float ZOOM = 45.0f;

    // An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices
    class Camera
    {
    public:
        // Constructor with vectors
        Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

        // Constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

        glm::mat4 GetViewMatrix();
        void ProcessKeyboard(Camera_Movement direction, float deltaTime);

        // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch);

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yOffset);

        float GetZoom();

        glm::vec3 GetPosition();

        glm::vec3 GetFront();

        glm::vec3 GetUp();

    private:
        // Camera Attributes
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 worldUp;

        // Eular Angles
        float yaw;
        float pitch;

        // Camera options
        float movementSpeed;
        float mouseSensitivity;
        float zoom;

        // Calculates the front vector from the Camera's (updated) Eular Angles
        void updateCameraVectors();
    };
}