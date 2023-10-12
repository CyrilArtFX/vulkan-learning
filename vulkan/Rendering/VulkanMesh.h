#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <vector>

#include <Utils/VulkanUtilities.h>

struct Model 
{
	glm::mat4 model;
};

class VulkanMesh
{
public:
	VulkanMesh(vk::PhysicalDevice physicalDeviceP, vk::Device deviceP, vk::Queue transferQueue, 
		vk::CommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, int texIdP);
	VulkanMesh() = default;
	~VulkanMesh() = default;

	size_t getVextexCount() { return vertexCount; }
	size_t getIndexCount() { return indexCount; }
	vk::Buffer getVertexBuffer() { return vertexBuffer; }
	vk::Buffer getIndexBuffer() { return indexBuffer; }
	Model getModel() const { return model; }
	void setModel(const glm::mat4& modelP);
	int getTexId() const { return texId; }

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

	Model model;
	int texId;

	void createVertexBuffer(vk::Queue transferQueue, vk::CommandPool transferCommandPool, std::vector<Vertex>* vertices);
	void createIndexBuffer(vk::Queue transferQueue, vk::CommandPool transferCommandPool, std::vector<uint32_t>* indices);
};