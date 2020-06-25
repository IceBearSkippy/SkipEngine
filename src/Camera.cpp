#include <Camera.h>

namespace Skip {

    Camera::Camera() 
        : _front(glm::vec3(0.0f, 0.0f, -1.0f)), _movementSpeed(SPEED), _mouseSensitivity(SENSITIVTY), _zoom(ZOOM) {
    }

    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : _front(glm::vec3(0.0f, 0.0f, -1.0f)), _movementSpeed(SPEED), _mouseSensitivity(SENSITIVTY), _zoom(ZOOM) {
        this->_position = position;
        this->_worldUp = up;
        this->_yaw = yaw;
        this->_pitch = pitch;
        this->updateCameraVectors();
    }

    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : _front(glm::vec3(0.0f, 0.0f, -1.0f)), _movementSpeed(SPEED), _mouseSensitivity(SENSITIVTY), _zoom(ZOOM) {
        this->_position = glm::vec3(posX, posY, posZ);
        this->_worldUp = glm::vec3(upX, upY, upZ);
        this->_yaw = yaw;
        this->_pitch = pitch;
        this->updateCameraVectors();
    }
    
    Camera::~Camera() {}

    

    glm::mat4 Camera::GetViewMatrix() {
        return glm::lookAt(this->_position, this->_position + this->_front, this->_up);
    }

    void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = this->_movementSpeed * deltaTime;

        if (direction == FORWARD) {
            this->_position += this->_front * velocity;
        }

        if (direction == BACKWARD) {
            this->_position -= this->_front * velocity;
        }

        if (direction == LEFT) {
            this->_position -= this->_right * velocity;
        }

        if (direction == RIGHT) {
            this->_position += this->_right * velocity;
        }
    }

    void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
        xOffset *= this->_mouseSensitivity;
        yOffset *= this->_mouseSensitivity;

        this->_yaw += xOffset;
        this->_pitch += yOffset;

        if (constrainPitch) {
            if (this->_pitch > 89.0f) {
                this->_pitch = 89.0f;
            }

            if (this->_pitch < -89.0f) {
                this->_pitch = -89.0f;
            }
        }

        this->updateCameraVectors();
    }

    void Camera::ProcessMouseScroll(float yOffset) {

    }

    float Camera::GetZoom() {
        return this->_zoom;
    }

    glm::vec3 Camera::GetPosition() {
        return this->_position;
    }

    glm::vec3 Camera::GetFront() {
        return this->_front;
    }
    glm::vec3 Camera::GetUp() {
        return this->_up;
    }

    void Camera::updateCameraVectors() {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
        front.y = sin(glm::radians(this->_pitch));
        front.z = sin(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
        this->_front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->_right = glm::normalize(glm::cross(this->_front, this->_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->_up = glm::normalize(glm::cross(this->_right, this->_front));
    }
}