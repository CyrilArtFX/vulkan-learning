#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <Utils/VulkanUtilities.h>
#include "VulkanMesh.h"

#include <stdexcept>
#include <vector>
#include <set>
#include <array>

struct MainDevice
{
	vk::PhysicalDevice physicalDevice;
	vk::Device logicalDevice;
};

struct ViewProjection 
{
	glm::mat4 projection;
	glm::mat4 view;
};

class VulkanRenderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();

	int init(GLFWwindow* windowP);
	void draw();
	void clean();

	void updateModel(int modelId, glm::mat4 modelP);



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

	std::vector<vk::Framebuffer> swapchainFramebuffers;
	vk::CommandPool graphicsCommandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	std::vector<vk::Semaphore> imageAvailable;
	std::vector<vk::Semaphore> renderFinished;
	const int MAX_FRAME_DRAWS = 2;			// Should be less than the number of swapchain images, here 3 (could cause bugs)
	int currentFrame = 0;
	std::vector<vk::Fence> drawFences;

	vk::DescriptorSetLayout descriptorSetLayout;
	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;

	std::vector<vk::Buffer> vpUniformBuffer;
	std::vector<vk::DeviceMemory> vpUniformBufferMemory;
	std::vector<vk::Buffer> modelUniformBufferDynamic;
	std::vector<vk::DeviceMemory> modelUniformBufferMemoryDynamic;


	std::vector<VulkanMesh> meshes;
	ViewProjection viewProj;

	const int MAX_OBJECTS = 2;
	VkDeviceSize minUniformBufferOffet;
	size_t modelUniformAlignement;
	Model* modelTransferSpace;

	vk::PushConstantRange pushConstantRange;

	// Depth
	vk::Image depthBufferImage;
	vk::DeviceMemory depthBufferImageMemory;
	vk::ImageView depthBufferImageView;

	//  textures
	std::vector<vk::Image> textureImages;
	std::vector<vk::ImageView> textureImageViews;
	std::vector<vk::DeviceMemory> textureImageMemory;
	vk::Sampler textureSampler;
	vk::DescriptorPool samplerDescriptorPool;
	vk::DescriptorSetLayout samplerDescriptorSetLayout;
	std::vector<vk::DescriptorSet> samplerDescriptorSets;





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

	//  buffers
	void createFramebuffers();
	void createGraphicsCommandPool();
	void createGraphicsCommandBuffers();
	void recordCommands(uint32_t currentImage);

	//  draw
	void createSynchronisation();


	void createDescriptorSetLayout();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void updateUniformBuffers(uint32_t imageIndex);
	void allocateDynamicBufferTransferSpace();
	void createPushConstantRange();

	//  depth
	void createDepthBufferImage();
	VkImage createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
		vk::ImageUsageFlags useFlags, vk::MemoryPropertyFlags propFlags, vk::DeviceMemory* imageMemory);
	vk::Format chooseSupportedFormat(const std::vector<vk::Format>& formats, vk::ImageTiling tiling,
		vk::FormatFeatureFlags featureFlags);


	//  textures
	stbi_uc* loadTextureFile(const std::string& filename, int* width, int* height, vk::DeviceSize* imageSize);
	int createTextureImage(const std::string& filename);
	int createTexture(const std::string & filename);
	void createTextureSampler();
	int createTextureDescriptor(vk::ImageView textureImageView);


public:
#ifdef NDEBUG
	static const bool enableValidationLayers = false;
#else
	static const bool enableValidationLayers = false; //  change this back to true when the pc will have vulkan correctly installed
#endif
	static const std::vector<const char*> validationLayers;
};

