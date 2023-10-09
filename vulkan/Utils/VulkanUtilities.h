#pragma once
#include <iostream>
#include <fstream>
#include <vulkan/vulkan.hpp>

#include <vector>
#include <string>


const std::vector<const char*> deviceExtensions
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


struct QueueFamilyIndices
{
	int graphicsFamily = -1; // Location of Graphics Queue Family
	int presentationFamily = -1; // Location of Presentation Queue Family

	bool isValid()
	{
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}


struct SwapchainDetails {
	// What the surface is capable of displaying, e.g. image size/extent
	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	// Vector of the image formats, e.g. RGBA
	std::vector<vk::SurfaceFormatKHR> formats;
	// Vector of presentation modes
	std::vector<vk::PresentModeKHR> presentationModes;
};


struct SwapchainImage
{
	vk::Image image;
	vk::ImageView imageView;
};


static std::vector<char> readShaderFile(const std::string& filename)
{
	// Open shader file
	// spv files are binary data, put the pointer at the end of the file to get its size
	std::ifstream file{ filename, std::ios::binary | std::ios::ate };
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open a file");
	}

	// -- Buffer preparation
	// Get the size through the position of the pointer
	size_t fileSize = (size_t)file.tellg();
	// Set file buffer to the file size
	std::vector<char> fileBuffer(fileSize);
	// Move in file to start of the file
	file.seekg(0);

	// Reading and closing
	file.read(fileBuffer.data(), fileSize);
	file.close();
	return fileBuffer;
}