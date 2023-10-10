#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <vector>

#include <Utils/VulkanUtilities.h>

class VulkanMesh
{
public:
	VulkanMesh(vk::PhysicalDevice physicalDeviceP, vk::Device deviceP, std::vector<Vertex>* vertices);
	VulkanMesh() = default;
	~VulkanMesh() = default;

	size_t getVextexCount() { return vertexCount; }
	vk::Buffer getVertexBuffer() { return vertexBuffer; }

	void destroyBuffers();

private:
	size_t vertexCount;
	vk::Buffer vertexBuffer;
	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::DeviceMemory vertexBufferMemory;

	void createVertexBuffer(std::vector<Vertex>* vertices);
	uint32_t findMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t allowedTypes, vk::MemoryPropertyFlags properties);
};