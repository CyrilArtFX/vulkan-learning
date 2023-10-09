#include "VulkanRenderer.h"

#include <set>


const std::vector<const char*> VulkanRenderer::validationLayers
{
	"VK_LAYER_KHRONOS_validation"
};


VulkanRenderer::VulkanRenderer()
{
}

VulkanRenderer::~VulkanRenderer()
{
}


int VulkanRenderer::init(GLFWwindow* windowP)
{
	window = windowP;

	try
	{
		createInstance();
		setupDebugMessenger();
		getPhysicalDevice();
	}
	catch (const std::runtime_error& e)
	{
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


void VulkanRenderer::createInstance()
{
	// Information about the application
	// This data is for developer convenience
	vk::ApplicationInfo appInfo{};

	// Name of the app
	appInfo.pApplicationName = "Vulkan App";
	// Version of the application
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

	// Custom engine name
	appInfo.pEngineName = "No Engine";
	// Custom engine version
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

	// Vulkan version (here 1.1)
	appInfo.apiVersion = VK_API_VERSION_1_1;

	// Everything we create will be created with a createInfo
	// Here, info about the vulkan creation
	vk::InstanceCreateInfo createInfo{};

	// createInfo.pNext // Extended information
	// createInfo.flags // Flags with bitfield
	// 
	// Application info from above
	createInfo.pApplicationInfo = &appInfo;

	// Setup extensions instance will use
	std::vector<const char*> instanceExtensions;
	uint32_t glfwExtensionsCount = 0; // Glfw may require multiple extensions
	std::vector<const char*> glfwExtensions = getRequiredExtensions();

	for (size_t i = 0; i < glfwExtensionsCount; ++i)
	{
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	// Check extensions
	if (!checkInstanceExtensionSupport(instanceExtensions))
	{
		throw std::runtime_error("VkInstance does not support required extensions");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();


	// Validation layers
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}


	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;


	// Finally create instance
	instance = vk::createInstance(createInfo);
}

bool VulkanRenderer::checkInstanceExtensionSupport(const std::vector<const char*>& checkExtensions)
{
	// Create the vector of extensions
	std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();

	// Check if given extensions are in list of available extensions
	for (const auto& checkExtension : checkExtensions)
	{
		bool hasExtension = false;
		for (const auto& extension : extensions)
		{
			if (strcmp(checkExtension, extension.extensionName) == 0)
			{
				hasExtension = true;
				break;
			}
		}
		if (!hasExtension) return false;
	}

	return true;
}

std::vector<const char*> VulkanRenderer::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;

	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) 
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanRenderer::getPhysicalDevice()
{
	// Get available physical device
	std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

	// If no devices available
	if (devices.size() == 0)
	{
		throw std::runtime_error("Can't find any GPU that supports vulkan");
	}

	// Get device valid for what we want to do
	for (const auto& device : devices)
	{
		if (checkDeviceSuitable(device))
		{
			mainDevice.physicalDevice = device;
			break;
		}
	}
}

bool VulkanRenderer::checkDeviceSuitable(vk::PhysicalDevice device)
{
	// Information about the device itself (ID, name, type, vendor, etc.)
	vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

	// Information about what the device can do (geom shader, tesselation, wide lines...)
	vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

	// For now we do nothing with this info
	QueueFamilyIndices indices = getQueueFamilies(device);
	return indices.isValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices;
	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

	// Go through each queue family and check it has at least one required type of queue
	int i = 0;

	for (const auto& queueFamily : queueFamilies)
	{
		// Check there is at least graphics queue
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.graphicsFamily = i;
		}

		if (indices.isValid()) break;

		++i;
	}

	return indices;
}


void VulkanRenderer::createLogicalDevice()
{
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// Vector for queue creation information, and set for family indices.
	// A set will only keep one indice if they are the same.
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { indices.graphicsFamily };

	// Queues the logical device needs to create and info to do so.
	for (int queueFamilyIndex : queueFamilyIndices)
	{
		vk::DeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		float priority = 1.0f;

		// Vulkan needs to know how to handle multiple queues. It uses priorities.
		// 1 is the highest priority.
		queueCreateInfo.pQueuePriorities = &priority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Logical device creation
	vk::DeviceCreateInfo deviceCreateInfo{};

	// Queues info
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

	// -- Validation layers are deprecated since Vulkan 1.1
	// Features
	// For now, no device features (tessellation etc.)
	vk::PhysicalDeviceFeatures deviceFeatures{};
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	// Create the logical device for the given physical device
	mainDevice.logicalDevice = mainDevice.physicalDevice.createDevice(deviceCreateInfo);

	// Ensure access to queues
	graphicsQueue = mainDevice.logicalDevice.getQueue(indices.graphicsFamily, 0);
}

bool VulkanRenderer::checkValidationLayerSupport()
{
	std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

	// Check if all of the layers in validation layers exist in the available layers
	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound) return false;
	}

	return true;
}


void VulkanRenderer::setupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug messenger.");
	}
}

void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

VkResult VulkanRenderer::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanRenderer::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}



void VulkanRenderer::clean()
{
	if (enableValidationLayers)
	{
		destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	mainDevice.logicalDevice.destroy();
	instance.destroy();
}
