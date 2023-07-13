// header for Vulkan
#include "vulkan.h"

void HelloTriangleApplication::run() {
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void HelloTriangleApplication::initWindow() {
	glfwInit();																					// glfw init

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);												// no ogl context
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);													// forbide resize window

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);						// create window (w, h, title, monitor, <ogl_para>)

}

void HelloTriangleApplication::creatInstance() {
	if (enableValidationLayers && !checkValidationLayerSupport())
		throw std::runtime_error("validation layers requested, but not available!");

	VkApplicationInfo appInfo{};																// application info which is optional
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};															// instance info which is necessary
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef __DEBUG__	// Checking for extension support 
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		std::cout << "available extensions:\n";
		for (const auto& extension : extensions) {
			std::cout << '\t' << extension.extensionName << '\n';
			bool extensionFound = false;
			for (const auto& requiredExtension : extensions)
				if (strcmp(requiredExtension.extensionName, extension.extensionName) == 0) {
					extensionFound = true;
					break;
				}
			std::cout << '\t' << (extensionFound ? "available" : "unavailable") << '\n';
		}
	}
#endif
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("failed to create instance!");
}

void HelloTriangleApplication::initVulkan() {
	creatInstance();
	setupDebugMessenger();
}

void HelloTriangleApplication::setupDebugMessenger() {
	if (!enableValidationLayers)return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);
	
	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}

}

void HelloTriangleApplication::mainLoop() {
	while (!glfwWindowShouldClose(window)) {													// check if window is closed
		glfwPollEvents();
	}
}

void HelloTriangleApplication::cleanup() {
	if (enableValidationLayers)
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);																	// close window
	glfwTerminate();																			// glfw close
}


