#pragma once

// CPP lib
#include <iostream>
#include <cstring>

// C standard should be replaced with more efficient codes
#include <stdexcept>
#include <cstdlib>
#include <limits>
#include <vector>
#include <map>
#include <optional>
#include <set>

// Vulkan define and include
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

// constant value
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// validation layers
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
#ifdef __DEBUG__
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

// queue struct
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

// Vulkan app class
class HelloTriangleApplication {
public:
	void run();
private:
	GLFWwindow* window;
	VkInstance instance;
	VkSurfaceKHR surface;

	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	void initWindow();
	void createInstance();
	void createSurface();

	void initVulkan();
	void setupDebugMessenger();
	void pickPhysicalDevice();
	void createLogicalDevice();

	void mainLoop();
	void cleanup();

private:
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();

	bool isDeviceSuitable(VkPhysicalDevice device);
	int rateDeviceSuitability(VkPhysicalDevice device);

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};

// Utils
#pragma region Utils
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);
#pragma endregion