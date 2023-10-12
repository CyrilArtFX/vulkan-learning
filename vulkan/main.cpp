#define DWORD unsigned int
#if defined(WIN32) || defined(_WIN32)
extern "C" { __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }
extern "C" {
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001; }
#else
extern "C" { int NvOptimusEnablement = 1; }
extern "C" { int AmdPowerXpressRequestHighPerformance = 1; }
#endif


#define STB_IMAGE_IMPLEMENTATION



#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <Rendering/VulkanRenderer.h>

#include <vector>
#include <string>
#include <stdexcept>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

using std::string;


GLFWwindow* window = nullptr;
VulkanRenderer vulkanRenderer;


void initWindow(string wName = "Vulkan", const int width = 800, const int height = 600)
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Glfw won't work with opengl
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}

void clean()
{
	vulkanRenderer.clean();
	glfwDestroyWindow(window);
	glfwTerminate();
}

int main()
{
	initWindow();
	if (vulkanRenderer.init(window) == EXIT_FAILURE) return EXIT_FAILURE;

	float angle = 0.0f;
	float deltaTime = 0.0f;
	float lastTime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		float now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		angle += 10.0 * deltaTime;
		if (angle > 360.0f) { angle -= 360.0f; }

		glm::mat4 firstModel(1.0f);
		glm::mat4 secondModel(1.0f);

		firstModel = glm::translate(firstModel, glm::vec3(0.0f, -1.0f, -2.0f));
		firstModel = glm::rotate(firstModel, glm::radians(angle), glm::vec3(0.0f, 1.0f, 1.0f));

		secondModel = glm::translate(secondModel, glm::vec3(0.0f, -1.0f, -1.0f));
		secondModel = glm::rotate(secondModel, glm::radians(-angle * 2), glm::vec3(0.0f, 0.0f, 1.0f));

		vulkanRenderer.updateModel(0, firstModel);
		vulkanRenderer.updateModel(1, secondModel);

		vulkanRenderer.draw();
	}

	clean();
	return 0;
}