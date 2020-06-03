#include <VulkanWindow.h>

using namespace std;

const uint32_t DEFAULT_WINDOW_WIDTH = 800;
const uint32_t DEFAULT_WINDOW_HEIGHT = 600;


namespace Skip {
	VulkanWindow::VulkanWindow() {
		_width = DEFAULT_WINDOW_WIDTH;
		_height = DEFAULT_WINDOW_HEIGHT;
		_title = "Skip it up";
	}

	VulkanWindow::VulkanWindow(uint32_t width, uint32_t height, char* title) {
		_width = width;
		_height = height;
		_title = title;
	}

	VulkanWindow::~VulkanWindow() {

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
		glfwSetFramebufferSizeCallback(_glfw, framebufferResizeCallback);
	}

	
	bool VulkanWindow::shouldClose() {
		return glfwWindowShouldClose(_glfw);
	}
}