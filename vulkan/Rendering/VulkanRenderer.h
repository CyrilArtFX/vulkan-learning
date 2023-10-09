#pragma once
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <Utils/VulkanUtilities.h>

#include <stdexcept>
#include <vector>
#include <set>
#include <array>

struct MainDevice
{
	vk::PhysicalDevice physicalDevice;
	vk::Device logicalDevice;
};

class VulkanRenderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();

	int init(GLFWwindow* windowP);
	void clean();

private:
	GLFWwindow* window;
	vk::Instance instance;
	MainDevice mainDevice;
	vk::Queue graphicsQueue;
	VkDebugUtilsMessengerEXT debugMessenger;

	vk::SurfaceKHR surface;
	vk::SwapchainKHR swapchain;
	vk::Queue presentationQueue;
	vk::Format swapchainImageFormat;
	vk::Extent2D swapchainExtent;
	std::vector<SwapchainImage> swapchainImages;

	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderPass;
	vk::Pipeline graphicsPipeline;



	//  instance
	void createInstance(); 
	bool checkInstanceExtensionSupport(const std::vector<const char*>& checkExtensions);
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();

	//  devices
	void getPhysicalDevice();
	bool checkDeviceSuitable(vk::PhysicalDevice device);
	QueueFamilyIndices getQueueFamilies(vk::PhysicalDevice device);
	void createLogicalDevice(); 

	//  debug
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	//  surface and swapchain
	vk::SurfaceKHR createSurface();
	void createSwapchain();
	SwapchainDetails getSwapchainDetails(vk::PhysicalDevice device);
	bool checkDeviceExtensionSupport(vk::PhysicalDevice device);

	vk::SurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
	vk::PresentModeKHR chooseBestPresentationMode(const std::vector<vk::PresentModeKHR>& presentationModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);
	vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlagBits aspectFlags);

	//  graphics pipeline
	void createGraphicsPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createRenderPass();



public:
#ifdef NDEBUG
	static const bool enableValidationLayers = false;
#else
	static const bool enableValidationLayers = false; //  change this back to true when the pc will have vulkan correctly installed
#endif
	static const std::vector<const char*> validationLayers;
};

