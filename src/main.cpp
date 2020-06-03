
#include <VulkanManager.h>;

#include <iostream>
using namespace std;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

Skip::VulkanWindow* window;
Skip::VulkanManager* vulkan;
int main()
{
	// Main is our application
	// We will interact with Vulkan manager and vulkan window
	// GLFW will live in VulkanWindow and have public access

	cout << "Hello CMake." << endl;
	return 0;
}
