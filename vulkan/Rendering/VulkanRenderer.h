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

	void createInstance(); 
	bool checkInstanceExtensionSupport(const std::vector<const char*>& checkExtensions);

	void getPhysicalDevice();
	bool checkDeviceSuitable(vk::PhysicalDevice device);
	QueueFamilyIndices getQueueFamilies(vk::PhysicalDevice device);

	void createLogicalDevice();
};

