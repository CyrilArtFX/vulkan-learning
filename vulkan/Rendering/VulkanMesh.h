#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <vector>

#include <Utils/VulkanUtilities.h>

class VulkanMesh
{
public:
	VulkanMesh(vk::PhysicalDevice physicalDeviceP, vk::Device deviceP, vk::Queue transferQueue, 
		vk::CommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices);
	VulkanMesh() = default;
	~VulkanMesh() = default;

	size_t getVextexCount() { return vertexCount; }
	size_t getIndexCount() { return indexCount; }
	vk::Buffer getVertexBuffer() { return vertexBuffer; }
	vk::Buffer getIndexBuffer() { return indexBuffer; }

	void destroyBuffers();

private:
	size_t vertexCount;
	size_t indexCount;
	vk::Buffer vertexBuffer;
	vk::Buffer indexBuffer;
	vk::DeviceMemory vertexBufferMemory;
	vk::DeviceMemory indexBufferMemory;

	vk::PhysicalDevice physicalDevice;
	vk::Device device;

	void createVertexBuffer(vk::Queue transferQueue, vk::CommandPool transferCommandPool, std::vector<Vertex>* vertices);
	void createIndexBuffer(vk::Queue transferQueue, vk::CommandPool transferCommandPool, std::vector<uint32_t>* indices);
};