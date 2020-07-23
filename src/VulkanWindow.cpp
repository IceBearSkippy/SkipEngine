#include <VulkanWindow.h>

const uint32_t DEFAULT_WINDOW_WIDTH = 1200;
const uint32_t DEFAULT_WINDOW_HEIGHT = 800;
const char* DEFAULT_WINDOW_NAME = "Skip";
bool keys[1024];

namespace Skip {
    
    bool firstMouse = true;
    float lastX = 400, lastY = 300;

    VulkanWindow::VulkanWindow() {
        _width = DEFAULT_WINDOW_WIDTH;
        _height = DEFAULT_WINDOW_HEIGHT;
        _title = DEFAULT_WINDOW_NAME;
        _scene = new SkipScene();
    }

    VulkanWindow::VulkanWindow(SkipScene* scene) {
        _width = DEFAULT_WINDOW_WIDTH;
        _height = DEFAULT_WINDOW_HEIGHT;
        _title = DEFAULT_WINDOW_NAME;
        _scene = scene;
    }
    VulkanWindow::VulkanWindow(uint32_t width, uint32_t height, char* title, SkipScene* scene) {
        _width = width;
        _height = height;
        _title = title;
        _scene = scene;
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
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        _glfw = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(_glfw, this);

        glfwSetKeyCallback(_glfw, this->KeyCallback);
        //glfwSetCursorPosCallback(_glfw, this->MouseCallback);
        glfwSetInputMode(_glfw, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetFramebufferSizeCallback(_glfw, framebufferResizeCallback);
    }

    void VulkanWindow::setupImGUI() {
        // TODO setup platform rendering and binding
        //      This will be called from swapchain to pass necessary items
        /*
        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = g_Instance;
        init_info.PhysicalDevice = g_PhysicalDevice;
        init_info.Device = g_Device;
        init_info.QueueFamily = g_QueueFamily;
        init_info.Queue = g_Queue;
        init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = g_DescriptorPool;
        init_info.Allocator = g_Allocator;
        init_info.MinImageCount = g_MinImageCount;
        init_info.ImageCount = wd->ImageCount;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);
        */
    }
    
    bool VulkanWindow::shouldClose() {
        return glfwWindowShouldClose(_glfw);
    }

    void VulkanWindow::processKeys(float deltaTime) {
        if (keys[GLFW_KEY_TAB]) {
            _cameraActive = !_cameraActive;
            if (_cameraActive) {
                glfwSetInputMode(_glfw, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetCursorPosCallback(_glfw, this->MouseCallback);
            } else {
                glfwSetInputMode(_glfw, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetCursorPosCallback(_glfw, nullptr);
                firstMouse = true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
            _scene->_camera->ProcessKeyboard(FORWARD, deltaTime);
        }

        if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
            _scene->_camera->ProcessKeyboard(BACKWARD, deltaTime);
        }

        if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
            _scene->_camera->ProcessKeyboard(LEFT, deltaTime);
        }

        if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
            _scene->_camera->ProcessKeyboard(RIGHT, deltaTime);
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
        if (vulkanWindow->_cameraActive) {
            Camera* camera = vulkanWindow->_scene->_camera;
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
}