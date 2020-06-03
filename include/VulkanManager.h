#include <vulkan/vulkan.h>

#include <VulkanWindow.h>

namespace Skip {

	class VulkanManager {
	public:
		VulkanManager(VulkanWindow window);
		~VulkanManager();


	private:
		VkInstance instance;
		VulkanWindow vkWindow;
	};


}
