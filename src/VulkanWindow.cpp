#include <VulkanWindow.h>

const uint32_t DEFAULT_WINDOW_WIDTH = 800;
const uint32_t DEFAULT_WINDOW_HEIGHT = 600;
const char* DEFAULT_WINDOW_NAME = "Skip";
bool keys[1024];

namespace Skip {
	
	bool firstMouse = true;
	float lastX = 400, lastY = 300;

	VulkanWindow::VulkanWindow() {
		_width = DEFAULT_WINDOW_WIDTH;
		_height = DEFAULT_WINDOW_HEIGHT;
		_title = DEFAULT_WINDOW_NAME;
		_camera = new Camera(glm::vec3(2.0f, 2.0f, 2.0f));
	}

	VulkanWindow::VulkanWindow(Camera* camera) {
		_width = DEFAULT_WINDOW_WIDTH;
		_height = DEFAULT_WINDOW_HEIGHT;
		_title = DEFAULT_WINDOW_NAME;
		_camera = camera;
	}
	VulkanWindow::VulkanWindow(uint32_t width, uint32_t height, char* title, Camera* camera) {
		_width = width;
		_height = height;
		_title = title;
		_camera = camera;
	}

	VulkanWindow::~VulkanWindow() {
		glfwDestroyWindow(_glfw);
		glfwTerminate();
	}

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));
		app->_framebufferResized = true;
	}

	void VulkanWindow::init() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		_glfw = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(_glfw, this);

		glfwSetKeyCallback(_glfw, this->KeyCallback);
		glfwSetCursorPosCallback(_glfw, this->MouseCallback);
		glfwSetInputMode(_glfw, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwSetFramebufferSizeCallback(_glfw, framebufferResizeCallback);
	}

	
	bool VulkanWindow::shouldClose() {
		return glfwWindowShouldClose(_glfw);
	}

	void VulkanWindow::processKeys(float deltaTime) {
		if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
			_camera->ProcessKeyboard(FORWARD, deltaTime);
		}

		if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
			_camera->ProcessKeyboard(BACKWARD, deltaTime);
		}

		if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
			_camera->ProcessKeyboard(LEFT, deltaTime);
		}

		if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
			_camera->ProcessKeyboard(RIGHT, deltaTime);
		}
	}

	void VulkanWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key >= 0 && key < 1024) {
			if (action == GLFW_PRESS) {
				keys[key] = true;
			}
			else if (action == GLFW_RELEASE) {
				keys[key] = false;
			}
		}
	}

	void VulkanWindow::MouseCallback(GLFWwindow* window, double xPos, double yPos) {
		VulkanWindow* vulkanWindow =
			static_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));
		Camera* camera = vulkanWindow->_camera;
		if (firstMouse) {
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}

		float xOffset = xPos - lastX;
		float yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

		lastX = xPos;
		lastY = yPos;

		camera->ProcessMouseMovement(xOffset, yOffset);
	}
}