#pragma once
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <Utils/VulkanUtilities.h>

#include <stdexcept>
#include <vector>

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

	void createInstance(); 
	bool checkInstanceExtensionSupport(const std::vector<const char*>& checkExtensions);
	std::vector<const char*> getRequiredExtensions();

	void getPhysicalDevice();
	bool checkDeviceSuitable(vk::PhysicalDevice device);
	QueueFamilyIndices getQueueFamilies(vk::PhysicalDevice device);

	void createLogicalDevice(); 

	bool checkValidationLayerSupport();
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult createDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	void destroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);


public:
#ifdef NDEBUG
	static const bool enableValidationLayers = false;
#else
	static const bool enableValidationLayers = false; //  change this back to true when the pc will have vulkan correctly installed
#endif
	static const std::vector<const char*> validationLayers;
};

