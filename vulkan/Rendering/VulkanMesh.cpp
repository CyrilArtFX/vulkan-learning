#include "VulkanMesh.h"

VulkanMesh::VulkanMesh(vk::PhysicalDevice physicalDeviceP, vk::Device deviceP, vk::Queue transferQueue,
	vk::CommandPool transferCommandPool, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices, int texIdP) :
	vertexCount(vertices->size()), indexCount(indices->size()), physicalDevice(physicalDeviceP), device(deviceP), texId(texIdP)
{
	createVertexBuffer(transferQueue, transferCommandPool, vertices);
	createIndexBuffer(transferQueue, transferCommandPool, indices);
	
	model.model = glm::mat4(1.0f);
}

void VulkanMesh::destroyBuffers()
{
	device.destroyBuffer(vertexBuffer, nullptr);
	device.freeMemory(vertexBufferMemory, nullptr);
	device.destroyBuffer(indexBuffer, nullptr);
	device.freeMemory(indexBufferMemory, nullptr);
}


void VulkanMesh::setModel(const glm::mat4& modelP)
{
	model.model = modelP;
}



void VulkanMesh::createVertexBuffer(vk::Queue transferQueue, vk::CommandPool transferCommandPool, std::vector<Vertex>* vertices)
{
	vk::DeviceSize bufferSize = sizeof(Vertex) * vertices->size();

	// Temporary buffer to stage vertex data before transfering to GPU
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;

	createBuffer(physicalDevice, device, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		&stagingBuffer, &stagingBufferMemory);

	// Map memory to staging buffer
	void* data;
	device.mapMemory(stagingBufferMemory, {}, bufferSize, {}, &data);
	memcpy(data, vertices->data(), static_cast<size_t>(bufferSize));
	device.unmapMemory(stagingBufferMemory);

	// Create buffer with vk::BufferUsageFlagBits::eTransferDst to mark as recipient
	// of transfer data Buffer memory need to be vk::MemoryPropertyFlagBits::eDeviceLocal
	// meaning memory is on GPU only and not CPU-accessible
	createBuffer(physicalDevice, device, bufferSize,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		&vertexBuffer, &vertexBufferMemory);

	// Copy staging buffer to vertex buffer on GPU
	copyBuffer(device, transferQueue, transferCommandPool, stagingBuffer, vertexBuffer, bufferSize);

	// Clean staging buffer
	device.destroyBuffer(stagingBuffer, nullptr);
	device.freeMemory(stagingBufferMemory, nullptr);
}

void VulkanMesh::createIndexBuffer(vk::Queue transferQueue, vk::CommandPool transferCommandPool, std::vector<uint32_t>* indices)
{
	vk::DeviceSize bufferSize = sizeof(uint32_t) * indices->size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	createBuffer(physicalDevice, device, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		&stagingBuffer, &stagingBufferMemory);

	void* data;
	device.mapMemory(stagingBufferMemory, {}, bufferSize, {}, &data);
	memcpy(data, indices->data(), static_cast<size_t>(bufferSize));
	device.unmapMemory(stagingBufferMemory);

	// This time with vk::BufferUsageFlagBits::eIndexBuffer,
	// &indexBuffer and &indexBufferMemory
	createBuffer(physicalDevice, device, bufferSize,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		&indexBuffer, &indexBufferMemory);

	// Copy to indexBuffer
	copyBuffer(device, transferQueue, transferCommandPool,
		stagingBuffer, indexBuffer, bufferSize);

	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingBufferMemory);
}
