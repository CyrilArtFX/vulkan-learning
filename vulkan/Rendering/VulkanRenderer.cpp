#include "VulkanRenderer.h"



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
		//  device 
		createInstance();
		setupDebugMessenger();
		surface = createSurface();
		getPhysicalDevice();
		createLogicalDevice();

		//  pipeline
		createSwapchain();
		createRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline();
		createFramebuffers();
		createGraphicsCommandPool();

		//  objects
		float aspectRatio = static_cast<float>(swapchainExtent.width) / static_cast<float>(swapchainExtent.height);
		mvp.projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
		mvp.view = glm::lookAt(glm::vec3(3.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.model = glm::mat4(1.0f);

		// In vulkan, y is downward, and for glm it is upward
		mvp.projection[1][1] *= -1;

		// -- Vertex data
		std::vector<Vertex> meshVertices1
		{
			{{-0.1f, -0.4f, 0.0f}, {1.0f, 0.0f, 0.0f}},	// 0
			{{-0.1f,  0.4f, 0.0f}, {0.0f, 1.0f, 0.0f}},	// 1
			{{-0.9f,  0.4f, 0.0f}, {0.0f, 0.0f, 1.0f}},	// 2
			{{-0.9f, -0.4f, 0.0f}, {1.0f, 1.0f, 0.0f}},	// 3
		};

		std::vector<Vertex> meshVertices2
		{
			{{ 0.9f, -0.4f, 0.0f}, {1.0f, 0.0f, 0.0f}},	// 0
			{{ 0.9f,  0.4f, 0.0f}, {0.0f, 1.0f, 0.0f}},	// 1
			{{ 0.1f,  0.4f, 0.0f}, {0.0f, 0.0f, 1.0f}},	// 2
			{{ 0.1f, -0.4f, 0.0f}, {1.0f, 1.0f, 0.0f}},	// 3
		};

		// -- Index data
		std::vector<uint32_t> meshIndices
		{
			0, 1, 2,
			2, 3, 0
		};

		VulkanMesh firstMesh = VulkanMesh(mainDevice.physicalDevice, mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, &meshVertices1, &meshIndices);
		VulkanMesh secondMesh = VulkanMesh(mainDevice.physicalDevice, mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, &meshVertices2, &meshIndices);
		meshes.push_back(firstMesh);
		meshes.push_back(secondMesh);

		//  data
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();

		//  commands
		createGraphicsCommandBuffers();
		recordCommands();
		createSynchronisation();
	}
	catch (const std::runtime_error& e)
	{
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void VulkanRenderer::draw()
{
	// 0. Freeze code until the drawFences[currentFrame] is open
	mainDevice.logicalDevice.waitForFences(drawFences[currentFrame], VK_TRUE, std::numeric_limits<uint32_t>::max());
	// When passing the fence, we close it behind us
	mainDevice.logicalDevice.resetFences(drawFences[currentFrame]);


	// 1. Get next available image to draw and set a semaphore to signal when we're finished with the image.
	uint32_t imageToBeDrawnIndex = (mainDevice.logicalDevice.acquireNextImageKHR(
		swapchain, std::numeric_limits<uint32_t>::max(), imageAvailable[currentFrame], VK_NULL_HANDLE)).value;
	updateUniformBuffer(imageToBeDrawnIndex);

	// 2. Submit command buffer to queue for execution, make sure it waits for the image to be signaled as available before drawing, 
	// and signals when it has finished rendering.
	vk::SubmitInfo submitInfo{};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailable[currentFrame];

	// Keep doing command buffer until imageAvailable is true
	vk::PipelineStageFlags waitStages[]{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
	// Stages to check semaphores at
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	// Command buffer to submit
	submitInfo.pCommandBuffers = &commandBuffers[imageToBeDrawnIndex];
	// Semaphores to signal when command buffer finishes
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinished[currentFrame];

	// When finished drawing, open the fence for the next submission
	graphicsQueue.submit(submitInfo, drawFences[currentFrame]);

	// 3. Present image to screen when it has signalled finished rendering
	vk::PresentInfoKHR presentInfo{};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinished[currentFrame];
	presentInfo.swapchainCount = 1;
	// Swapchains to present to
	presentInfo.pSwapchains = &swapchain;
	// Index of images in swapchains to present
	presentInfo.pImageIndices = &imageToBeDrawnIndex;

	presentationQueue.presentKHR(presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAME_DRAWS;
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
	std::vector<const char*> instanceExtensions = getRequiredExtensions();

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

	QueueFamilyIndices indices = getQueueFamilies(device);
	bool extensionSupported = checkDeviceExtensionSupport(device);

	bool swapchainValid = false;
	if (extensionSupported)
	{
		SwapchainDetails swapchainDetails = getSwapchainDetails(device);
		swapchainValid = !swapchainDetails.presentationModes.empty()
			&& !swapchainDetails.formats.empty();
	}

	return indices.isValid() && extensionSupported && swapchainValid;
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

		// Check if queue family support presentation
		VkBool32 presentationSupport = device.getSurfaceSupportKHR(static_cast<uint32_t>(indices.graphicsFamily), surface);

		if (queueFamily.queueCount > 0 && presentationSupport)
		{
			indices.presentationFamily = i;
		}

		if (indices.isValid()) break;

		++i;
	}

	return indices;
}

bool VulkanRenderer::checkDeviceExtensionSupport(vk::PhysicalDevice device)
{
	std::vector<vk::ExtensionProperties> extensions = device.enumerateDeviceExtensionProperties();

	for (const auto& deviceExtension : deviceExtensions)
	{
		bool hasExtension = false;

		for (const auto& extension : extensions)
		{
			if (strcmp(deviceExtension, extension.extensionName) == 0)
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension) return false;
	}

	return true;
}


void VulkanRenderer::createLogicalDevice()
{
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// Vector for queue creation information, and set for family indices.
	// A set will only keep one indice if they are the same.
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { indices.graphicsFamily, indices.presentationFamily };

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

	// Extensions info
	// Device extensions, different from instance extensions
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();


	// -- Validation layers are deprecated since Vulkan 1.1
	// Features
	// For now, no device features (tessellation etc.)
	vk::PhysicalDeviceFeatures deviceFeatures{};
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;


	// Create the logical device for the given physical device
	mainDevice.logicalDevice = mainDevice.physicalDevice.createDevice(deviceCreateInfo);

	// Ensure access to queues
	graphicsQueue = mainDevice.logicalDevice.getQueue(indices.graphicsFamily, 0);
	presentationQueue = mainDevice.logicalDevice.getQueue(indices.presentationFamily, 0);
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

vk::SurfaceKHR VulkanRenderer::createSurface()
{
	// Create a surface relatively to our window
	VkSurfaceKHR _surface;

	VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &_surface);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a vulkan surface.");
	}

	return vk::SurfaceKHR(_surface);
}

void VulkanRenderer::createSwapchain()
{
	// We will pick best settings for the swapchain
	SwapchainDetails swapchainDetails = getSwapchainDetails(mainDevice.physicalDevice);
	vk::SurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapchainDetails.formats);
	vk::PresentModeKHR presentationMode = chooseBestPresentationMode(
		swapchainDetails.presentationModes);
	vk::Extent2D extent = chooseSwapExtent(swapchainDetails.surfaceCapabilities);

	// Setup the swap chain info
	vk::SwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.imageFormat = surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainCreateInfo.presentMode = presentationMode;
	swapchainCreateInfo.imageExtent = extent;

	// Minimal number of image in our swapchain. We will use one
	// more than the minimum to enable triple-buffering.
	uint32_t imageCount = swapchainDetails.surfaceCapabilities.minImageCount + 1;

	if (swapchainDetails.surfaceCapabilities.maxImageCount > 0 // Not limitless
		&& swapchainDetails.surfaceCapabilities.maxImageCount < imageCount)
	{
		imageCount = swapchainDetails.surfaceCapabilities.maxImageCount;
	}
	swapchainCreateInfo.minImageCount = imageCount;


	// Number of layers for each image in swapchain
	swapchainCreateInfo.imageArrayLayers = 1;

	// What attachment go with the image (e.g. depth, stencil...). Here, just color.
	swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

	// Transform to perform on swapchain images
	swapchainCreateInfo.preTransform = swapchainDetails.surfaceCapabilities.currentTransform;

	// Handles blending with other windows. Here we don't blend.
	swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

	// Whether to clip parts of the image not in view (e.g. when an other window overlaps)
	swapchainCreateInfo.clipped = VK_TRUE;

	// Queue management
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);
	uint32_t queueFamilyIndices[]{ (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentationFamily };

	// If graphics and presentation families are different, share images between them
	if (indices.graphicsFamily != indices.presentationFamily)
	{
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	// When you want to pass old swapchain responsibilities when destroying it,
	// e.g. when you want to resize window, use this
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	// Create swapchain
	swapchain = mainDevice.logicalDevice.createSwapchainKHR(swapchainCreateInfo);

	// Store for later use
	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;

	// Get the swapchain images
	std::vector<vk::Image> images = mainDevice.logicalDevice.getSwapchainImagesKHR(swapchain);
	for (VkImage image : images)	// We are using handles, not values
	{
		SwapchainImage swapchainImage{};
		swapchainImage.image = image;

		// Create image view
		swapchainImage.imageView = createImageView(image, swapchainImageFormat, vk::ImageAspectFlagBits::eColor);

		swapchainImages.push_back(swapchainImage);
	}
}

SwapchainDetails VulkanRenderer::getSwapchainDetails(vk::PhysicalDevice device)
{
	SwapchainDetails swapchainDetails;
	// Capabilities
	swapchainDetails.surfaceCapabilities = device.getSurfaceCapabilitiesKHR(surface);
	// Formats
	swapchainDetails.formats = device.getSurfaceFormatsKHR(surface);
	// Presentation modes
	swapchainDetails.presentationModes = device.getSurfacePresentModesKHR(surface);

	return swapchainDetails;
}

vk::SurfaceFormatKHR VulkanRenderer::chooseBestSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
{
	// We will use RGBA 32bits normalized and SRGG non linear colorspace
	if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
	{
		// All formats available by convention
		return { vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (auto& format : formats)
	{
		if (format.format == vk::Format::eR8G8B8A8Unorm
			&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return format;
		}
	}

	// Return first format if we have not our chosen format
	return formats[0];
}

vk::PresentModeKHR VulkanRenderer::chooseBestPresentationMode(const std::vector<vk::PresentModeKHR>& presentationModes)
{
	// We will use mail box presentation mode
	for (const auto& presentationMode : presentationModes)
	{
		if (presentationMode == vk::PresentModeKHR::eMailbox)
		{
			return presentationMode;
		}
	}

	// Part of the Vulkan spec, so have to be available
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanRenderer::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities)
{
	// Rigid extents
	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}

	// Extents can vary
	else
	{
		// Create new extent using window size
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		vk::Extent2D newExtent{};
		newExtent.width = static_cast<uint32_t>(width);
		newExtent.height = static_cast<uint32_t>(height);

		// Sarface also defines max and min, so make sure we are within boundaries
		newExtent.width = std::max(surfaceCapabilities.minImageExtent.width,
			std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
		newExtent.height = std::max(surfaceCapabilities.minImageExtent.height,
			std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));
		return newExtent;
	}
}

vk::ImageView VulkanRenderer::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlagBits aspectFlags)
{
	vk::ImageViewCreateInfo viewCreateInfo{};
	viewCreateInfo.image = image;

	// Other formats can be used for cubemaps etc.
	viewCreateInfo.viewType = vk::ImageViewType::e2D;
	// Can be used for depth for instance
	viewCreateInfo.format = format;

	// Swizzle used to remap color values. Here we keep the same.
	viewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
	viewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
	viewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
	viewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;


	// Subresources allow the view to view only a part of an image
	// Here we want to see the image under the aspect of colors
	viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	// Start mipmap level to view from
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	// Number of mipmap level to view
	viewCreateInfo.subresourceRange.levelCount = 1;
	// Start array level to view from
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	// Number of array levels to view
	viewCreateInfo.subresourceRange.layerCount = 1;

	// Create image view
	vk::ImageView imageView = mainDevice.logicalDevice.createImageView(viewCreateInfo);
	return imageView;
}


void VulkanRenderer::createGraphicsPipeline()
{
	// Read shader code and format it through a shader module
	auto vertexShaderCode = readShaderFile("shaders/vert.spv");
	auto fragmentShaderCode = readShaderFile("shaders/frag.spv");
	vk::ShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
	vk::ShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);


	// -- SHADER STAGE CREATION INFO --
	// Vertex stage creation info
	vk::PipelineShaderStageCreateInfo vertexShaderCreateInfo{};
	// Used to know which shader
	vertexShaderCreateInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertexShaderCreateInfo.module = vertexShaderModule;
	// Pointer to the start function in the shader
	vertexShaderCreateInfo.pName = "main";
	// Fragment stage creation info
	vk::PipelineShaderStageCreateInfo fragmentShaderCreateInfo{};
	fragmentShaderCreateInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragmentShaderCreateInfo.module = fragmentShaderModule;
	fragmentShaderCreateInfo.pName = "main";
	// Graphics pipeline requires an array of shader create info
	vk::PipelineShaderStageCreateInfo shaderStages[]{ vertexShaderCreateInfo, fragmentShaderCreateInfo };


	// Vertex description
	// -- Binding, data layout
	vk::VertexInputBindingDescription bindingDescription{};
	// Binding position. Can bind multiple streams of data.
	bindingDescription.binding = 0;
	// Size of a single vertex data object, like in OpenGL
	bindingDescription.stride = sizeof(Vertex);
	// How ot move between data after each vertex.
	// vk::VertexInputRate::eVertex: move onto next vertex
	// vk::VertexInputRate::eInstance: move to a vertex for the next instance.
	// Draw each first vertex of each instance, then the next vertex etc.
	bindingDescription.inputRate = vk::VertexInputRate::eVertex;

	// Different attributes
	std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions;

	// Position attributes
	attributeDescriptions[0].binding = 0; // -- Binding of first attribute. Relate to binding description.
	attributeDescriptions[0].location = 0; // Location in shader
	attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat; // Format and size of the data(here: vec3)
	attributeDescriptions[0].offset = offsetof(Vertex, pos); // Offset of data in vertex, like in OpenGL. The offset function automatically find it.

	// Color attributes
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	// -- VERTEX INPUT STAGE --
	vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
	vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


	// -- INPUT ASSEMBLY --
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
	// How to assemble vertices
	inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
	// When you want to restart a primitive, e.g. with a strip
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;


	// -- VIEWPORT AND SCISSOR --
	// Create a viewport info struct
	vk::Viewport viewport{};
	viewport.x = 0.0f;		// X start coordinate
	viewport.y = 0.0f;		// Y start coordinate
	viewport.width = (float)swapchainExtent.width;		// Width of viewport
	viewport.height = (float)swapchainExtent.height;	// Height of viewport
	viewport.minDepth = 0.0f;				// Min framebuffer depth
	viewport.maxDepth = 1.0f;				// Max framebuffer depth

	// Create a scissor info struct, everything outside is cut
	vk::Rect2D scissor{};
	scissor.offset = vk::Offset2D{ 0, 0 };
	scissor.extent = swapchainExtent;

	vk::PipelineViewportStateCreateInfo viewportStateCreateInfo{};
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = &scissor;


	// -- DYNAMIC STATE --
	// This will be alterable, so you don't have to create an entire pipeline when you want to change
	// parameters. We won't use this feature, this is an example.
	/*
	vector<vk::DynamicState> dynamicStateEnables;
	// Viewport can be resized in the command buffer
	// with vkCmdSetViewport(commandBuffer, 0, 1, &newViewport);
	dynamicStateEnables.push_back(vk::DynamicState::eViewport);
	// Scissors can be resized in the command buffer
	// with vkCmdSetScissor(commandBuffer, 0, 1, &newScissor);
	dynamicStateEnables.push_back(vk::DynamicState::eScissor);

	vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
	dynamicStateCreateInfo.dynamicStateCount =
		static_cast<uint32_t>(dynamicStateEnables.size());
	dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();
	*/


	// -- RASTERIZER --
	vk::PipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
	// Treat elements beyond the far plane like being on the far place,
	// needs a GPU device feature
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	// Whether to discard data and skip rasterizer. When you want
	// a pipeline without framebuffer.
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	// How to handle filling points between vertices. Here, considers things inside
	// the polygon as a fragment. VK_POLYGON_MODE_LINE will consider element inside
	// polygones being empty (no fragment). May require a device feature.
	rasterizerCreateInfo.polygonMode = vk::PolygonMode::eFill;
	// How thick should line be when drawn
	rasterizerCreateInfo.lineWidth = 1.0f;
	// Culling. Do not draw back of polygons
	rasterizerCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
	// Widing to know the front face of a polygon
	rasterizerCreateInfo.frontFace = vk::FrontFace::eCounterClockwise;
	// Whether to add a depth offset to fragments. Good for stopping
	// "shadow acne" in shadow mapping. Is set, need to set 3 other values.
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;


	// -- MULTISAMPLING --
	// Not for textures, only for edges
	vk::PipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
	// Enable multisample shading or not
	multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
	// Number of samples to use per fragment
	multisamplingCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;


	// -- BLENDING --
	// How to blend a new color being written to the fragment, with the old value
	vk::PipelineColorBlendStateCreateInfo colorBlendingCreateInfo{};
	// Alternative to usual blending calculation
	colorBlendingCreateInfo.logicOpEnable = VK_FALSE;

	// Enable blending and choose colors to apply blending to
	vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
		vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
		vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_TRUE;

	// Blending equation:
	// (srcColorBlendFactor * new color) colorBlendOp (dstColorBlendFactor * old color)
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;

	// Replace the old alpha with the new one: (1 * new alpha) + (0 * old alpha)
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

	colorBlendingCreateInfo.attachmentCount = 1;
	colorBlendingCreateInfo.pAttachments = &colorBlendAttachment;


	// -- PIPELINE LAYOUT --
	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

	// Create pipeline layout
	pipelineLayout = mainDevice.logicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);



	// -- DEPTH STENCIL TESTING --
	// TODO: Set up depth stencil testing


	// -- PASSES --
	// Passes are composed of a sequence of subpasses that can pass data from one to another

	// -- GRAPHICS PIPELINE CREATION --
	vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
	graphicsPipelineCreateInfo.stageCount = 2;
	graphicsPipelineCreateInfo.pStages = shaderStages;
	graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState = nullptr;
	graphicsPipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &colorBlendingCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
	graphicsPipelineCreateInfo.layout = pipelineLayout;
	// Renderpass description the pipeline is compatible with.
	// This pipeline will be used by the render pass.
	graphicsPipelineCreateInfo.renderPass = renderPass;
	// Subpass of render pass to use with pipeline. Usually one pipeline by subpass.
	graphicsPipelineCreateInfo.subpass = 0;
	// When you want to derivate a pipeline from an other pipeline OR
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	// Index of pipeline being created to derive from (in case of creating multiple at once)
	graphicsPipelineCreateInfo.basePipelineIndex = -1;

	// The handle is a cache when you want to save your pipeline to create an other later
	auto result = mainDevice.logicalDevice.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
	// We could have used createGraphicsPipelines to create multiple pipelines at once.
	if (result.result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Cound not create a graphics pipeline");
	}
	graphicsPipeline = result.value;


	// Destroy shader modules
	mainDevice.logicalDevice.destroyShaderModule(fragmentShaderModule);
	mainDevice.logicalDevice.destroyShaderModule(vertexShaderModule);
}

VkShaderModule VulkanRenderer::createShaderModule(const std::vector<char>& code)
{
	vk::ShaderModuleCreateInfo shaderModuleCreateInfo{};
	shaderModuleCreateInfo.codeSize = code.size();
	// Conversion between pointer types with reinterpret_cast
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	vk::ShaderModule shaderModule =
		mainDevice.logicalDevice.createShaderModule(shaderModuleCreateInfo);
	return shaderModule;
}

void VulkanRenderer::createRenderPass()
{
	vk::RenderPassCreateInfo renderPassCreateInfo{};

	// Attachement description : describe color buffer output, depth buffer output...
	// e.g. (location = 0) in the fragment shader is the first attachment
	vk::AttachmentDescription colorAttachment{};
	// Format to use for attachment
	colorAttachment.format = swapchainImageFormat;
	// Number of samples to write for multisampling
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	// What to do with attachement before renderer. Here, clear when we start the render pass.
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	// What to do with attachement after renderer. Here, store the render pass.
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	// What to do with stencil before renderer. Here, don't care, we don't use stencil.
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	// What to do with stencil after renderer. Here, don't care, we don't use stencil.
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

	// Framebuffer images will be stored as an image, but image can have different layouts
	// to give optimal use for certain operations
	// Image data layout before render pass starts
	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	// Image data layout after render pass
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;

	// Attachment reference uses an attachment index that refers to index
	// in the attachement list passed to renderPassCreateInfo
	vk::AttachmentReference colorAttachmentReference{};
	colorAttachmentReference.attachment = 0;
	// Layout of the subpass (between initial and final layout)
	colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

	// Subpass description, will reference attachements
	vk::SubpassDescription subpass{};
	// Pipeline type the subpass will be bound to.
	// Could be compute pipeline, or nvidia raytracing...
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;

	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;

	// Subpass dependencies: transitions between subpasses + from the last subpass to what
	// happens after. Need to determine when layout transitions occur using subpass
	// dependencies. Will define implicitly layout transitions.
	std::array<vk::SubpassDependency, 2> subpassDependencies;
	// -- From layout undefined to color attachment optimal
	// ---- Transition must happens after
	// External: from outside the subpasses
	subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	// Which stage of the pipeline has to happen before
	subpassDependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	subpassDependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
	// ---- But must happens before
	// Conversion should happen before the first subpass starts
	subpassDependencies[0].dstSubpass = 0;
	subpassDependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	// ...and before the color attachment attempts to read or write
	subpassDependencies[0].dstAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
	subpassDependencies[0].dependencyFlags = {};	// No dependency flag

	// -- From layout color attachment optimal to image layout present
	// ---- Transition must happens after
	subpassDependencies[1].srcSubpass = 0;
	subpassDependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	subpassDependencies[1].srcAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
	// ---- But must happens before
	subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
	subpassDependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
	subpassDependencies[1].dependencyFlags = vk::DependencyFlags();

	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
	renderPassCreateInfo.pDependencies = subpassDependencies.data();

	renderPass = mainDevice.logicalDevice.createRenderPass(renderPassCreateInfo);
}

void VulkanRenderer::createFramebuffers()
{
	// Create one framebuffer for each swapchain image
	swapchainFramebuffers.resize(swapchainImages.size());
	for (size_t i = 0; i < swapchainFramebuffers.size(); ++i)
	{
		// Setup attachments
		std::array<vk::ImageView, 1> attachments{ swapchainImages[i].imageView };

		// Create info
		vk::FramebufferCreateInfo framebufferCreateInfo{};
		// Render pass layout the framebuffer will be used with
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		// List of attachments (1:1 with render pass, thanks to variable i)
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = swapchainExtent.width;
		framebufferCreateInfo.height = swapchainExtent.height;
		// Framebuffer layers
		framebufferCreateInfo.layers = 1;

		swapchainFramebuffers[i] = mainDevice.logicalDevice.createFramebuffer(framebufferCreateInfo);
	}
}

void VulkanRenderer::createGraphicsCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = getQueueFamilies(mainDevice.physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	// Queue family type that buffers from this command pool will use
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	graphicsCommandPool = mainDevice.logicalDevice.createCommandPool(poolInfo);
}

void VulkanRenderer::createGraphicsCommandBuffers()
{
	// Create one command buffer for each framebuffer
	commandBuffers.resize(swapchainFramebuffers.size());

	vk::CommandBufferAllocateInfo commandBufferAllocInfo{};
	// We are using a pool
	commandBufferAllocInfo.commandPool = graphicsCommandPool;
	commandBufferAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	// Primary means the command buffer will submit directly to a queue.
	// Secondary cannot be called by a queue, but by an other primary command
	// buffer, via vkCmdExecuteCommands.
	commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;

	commandBuffers = mainDevice.logicalDevice.allocateCommandBuffers(commandBufferAllocInfo);
}

void VulkanRenderer::recordCommands()
{
	// How to begin each command buffer
	vk::CommandBufferBeginInfo commandBufferBeginInfo{};
	// Buffer can be resubmited when it has already been submited
	//commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

	// Information about how to being a render pass (only for graphical apps)
	vk::RenderPassBeginInfo renderPassBeginInfo{};
	// Render pass to begin
	renderPassBeginInfo.renderPass = renderPass;
	// Start point of render pass in pixel
	renderPassBeginInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
	// Size of region to run render pass on
	renderPassBeginInfo.renderArea.extent = swapchainExtent;

	vk::ClearValue clearValues{};
	std::array<float, 4> colors{ 0.6f, 0.65f, 0.4f, 1.0f };
	clearValues.color = vk::ClearColorValue{ colors };
	renderPassBeginInfo.pClearValues = &clearValues;
	renderPassBeginInfo.clearValueCount = 1;

	for (size_t i = 0; i < commandBuffers.size(); ++i)
	{
		// Because 1-to-1 relationship
		renderPassBeginInfo.framebuffer = swapchainFramebuffers[i];

		// Start recording commands to command buffer
		commandBuffers[i].begin(commandBufferBeginInfo);

		// Begin render pass
		// All draw commands inline (no secondary command buffers)
		commandBuffers[i].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

		// Bind pipeline to be used in render pass, you could switch pipelines for different subpasses
		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

		// Draw all meshes
		for (size_t j = 0; j < meshes.size(); ++j)
		{
			// Bind vertex buffer
			vk::Buffer vertexBuffers[] = { meshes[j].getVertexBuffer() };
			vk::DeviceSize offsets[] = { 0 };
			commandBuffers[i].bindVertexBuffers(0, vertexBuffers, offsets);

			// Bind index buffer
			commandBuffers[i].bindIndexBuffer(meshes[j].getIndexBuffer(), 0, vk::IndexType::eUint32);

			// Bind descriptor sets
			commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

			// Execute pipeline
			commandBuffers[i].drawIndexed(static_cast<uint32_t>(meshes[j].getIndexCount()), 1, 0, 0, 0);
		}

		// End render pass
		commandBuffers[i].endRenderPass();

		// Stop recordind to command buffer
		commandBuffers[i].end();
	}
}

void VulkanRenderer::createSynchronisation()
{
	imageAvailable.resize(MAX_FRAME_DRAWS);
	renderFinished.resize(MAX_FRAME_DRAWS);
	drawFences.resize(MAX_FRAME_DRAWS);

	// Semaphore creation info
	vk::SemaphoreCreateInfo semaphoreCreateInfo{};

	// Fence creation info
	vk::FenceCreateInfo fenceCreateInfo{};
	// Fence starts open
	fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

	for (size_t i = 0; i < MAX_FRAME_DRAWS; ++i)
	{
		imageAvailable[i] = mainDevice.logicalDevice.createSemaphore(semaphoreCreateInfo);
		renderFinished[i] = mainDevice.logicalDevice.createSemaphore(semaphoreCreateInfo);
		drawFences[i] = mainDevice.logicalDevice.createFence(fenceCreateInfo);
	}
}

void VulkanRenderer::createDescriptorSetLayout()
{
	// MVP binding information
	vk::DescriptorSetLayoutBinding mvpLayoutBinding;
	// Binding number in shader
	mvpLayoutBinding.binding = 0;
	// Type of descriptor (uniform, dynamic uniform, samples...)
	mvpLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	// Number of descriptors for binding
	mvpLayoutBinding.descriptorCount = 1;
	// Shader stage to bind to (here: vertex shader)
	mvpLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	// For textures : can make sample data un changeable
	mvpLayoutBinding.pImmutableSamplers = nullptr;

	// Descriptor set layout with given binding
	vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.bindingCount = 1;
	layoutCreateInfo.pBindings = &mvpLayoutBinding;

	// Create descriptor set layout
	descriptorSetLayout = mainDevice.logicalDevice.createDescriptorSetLayout(layoutCreateInfo);
}

void VulkanRenderer::createUniformBuffers()
{
	// Buffer size will be size of all 3 variables
	vk::DeviceSize bufferSize = sizeof(MVP);

	// One uniform buffer for each image / each command buffer
	uniformBuffer.resize(swapchainImages.size());
	uniformBufferMemory.resize(swapchainImages.size());

	// Create uniform buffers
	for (size_t i = 0; i < swapchainImages.size(); ++i)
	{
		createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, bufferSize,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			&uniformBuffer[i], &uniformBufferMemory[i]);
	}
}

void VulkanRenderer::createDescriptorPool()
{
	// One descriptor in the pool for each image
	vk::DescriptorPoolSize poolSize{};
	poolSize.descriptorCount = static_cast<uint32_t>(uniformBuffer.size());

	// One descriptor set that contains one descriptor
	vk::DescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.maxSets = static_cast<uint32_t>(uniformBuffer.size());
	poolCreateInfo.poolSizeCount = 1;
	poolCreateInfo.pPoolSizes = &poolSize;

	// Create pool
	descriptorPool = mainDevice.logicalDevice.createDescriptorPool(poolCreateInfo);
}

void VulkanRenderer::createDescriptorSets()
{
	// One descriptor set for every image/buffer
	descriptorSets.resize(uniformBuffer.size());

	// We want the same layout for the right number of descriptor sets
	std::vector<vk::DescriptorSetLayout> setLayouts(uniformBuffer.size(), descriptorSetLayout);

	// Allocation from the pool
	vk::DescriptorSetAllocateInfo setAllocInfo{};
	setAllocInfo.descriptorPool = descriptorPool;
	setAllocInfo.descriptorSetCount = static_cast<uint32_t>(uniformBuffer.size());
	setAllocInfo.pSetLayouts = setLayouts.data();

	// Allocate multiple descriptor sets
	mainDevice.logicalDevice.allocateDescriptorSets(&setAllocInfo, descriptorSets.data());

	// We have a connection between descriptor set layouts and descriptor sets,
	// but we don't know how link descriptor sets and the uniform buffers.

	// Update all of descriptor set buffer bindings
	for (size_t i = 0; i < uniformBuffer.size(); ++i)
	{
		// Description of the buffer and data offset
		vk::DescriptorBufferInfo mvpBufferInfo{};
		// Buffer to get data from
		mvpBufferInfo.buffer = uniformBuffer[i];
		// We bind the whole data
		mvpBufferInfo.offset = 0;
		// Size of data
		mvpBufferInfo.range = sizeof(MVP);

		// Data about connection between binding and buffer
		vk::WriteDescriptorSet mvpSetWrite{};
		// Descriptor sets to update
		mvpSetWrite.dstSet = descriptorSets[i];
		// Binding to update (matches with shader binding)
		mvpSetWrite.dstBinding = 0;
		// Index in array to update
		mvpSetWrite.dstArrayElement = 0;
		mvpSetWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
		// Amount of descriptor sets to update
		mvpSetWrite.descriptorCount = 1;
		// Information about buffer data to bind
		mvpSetWrite.pBufferInfo = &mvpBufferInfo;

		// Update descriptor set with new buffer/binding info
		mainDevice.logicalDevice.updateDescriptorSets(1, &mvpSetWrite, 0, nullptr);
	}	
}

void VulkanRenderer::updateUniformBuffer(uint32_t imageIndex)
{
	void* data;
	mainDevice.logicalDevice.mapMemory(uniformBufferMemory[imageIndex], {}, sizeof(MVP), {}, &data);
	memcpy(data, &mvp, sizeof(MVP));
	mainDevice.logicalDevice.unmapMemory(uniformBufferMemory[imageIndex]);
}





void VulkanRenderer::clean()
{
	mainDevice.logicalDevice.waitIdle();

	mainDevice.logicalDevice.destroyDescriptorPool(descriptorPool);

	mainDevice.logicalDevice.destroyDescriptorSetLayout(descriptorSetLayout);
	for (size_t i = 0; i < uniformBuffer.size(); ++i)
	{
		mainDevice.logicalDevice.destroyBuffer(uniformBuffer[i]);
		mainDevice.logicalDevice.freeMemory(uniformBufferMemory[i]);
	}

	for (auto& mesh : meshes)
	{
		mesh.destroyBuffers();
	}

	for (size_t i = 0; i < MAX_FRAME_DRAWS; ++i)
	{
		mainDevice.logicalDevice.destroySemaphore(renderFinished[i]);
		mainDevice.logicalDevice.destroySemaphore(imageAvailable[i]); 
		mainDevice.logicalDevice.destroyFence(drawFences[i]); 
	}

	mainDevice.logicalDevice.destroyCommandPool(graphicsCommandPool);
	for (auto framebuffer : swapchainFramebuffers)
	{
		mainDevice.logicalDevice.destroyFramebuffer(framebuffer);
	}

	mainDevice.logicalDevice.destroyPipeline(graphicsPipeline);
	mainDevice.logicalDevice.destroyPipelineLayout(pipelineLayout);
	mainDevice.logicalDevice.destroyRenderPass(renderPass);

	for (auto image : swapchainImages)
	{
		mainDevice.logicalDevice.destroyImageView(image.imageView);
	}

	mainDevice.logicalDevice.destroySwapchainKHR(swapchain);
	instance.destroySurfaceKHR(surface);

	if (enableValidationLayers)
	{
		destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	mainDevice.logicalDevice.destroy();
	instance.destroy();
}


void VulkanRenderer::updateModel(glm::mat4 modelP)
{
	mvp.model = modelP;
}