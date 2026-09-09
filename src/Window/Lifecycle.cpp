#include <iostream>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <lumi_render/Window.hpp>

int Window::windowCount = 0;

Window::Window(int width, int height, bool fullscreen) : width{width}, height{height}, fullscreen{fullscreen} {
	if (windowCount == 0 && !glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	++windowCount;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (fullscreen) {
		window = glfwCreateWindow(
			width,
			height,
			"OpenGL",
			glfwGetPrimaryMonitor(),
			nullptr
		);
	} else {
		window = glfwCreateWindow(
			width,
			height,
			"OpenGL",
			nullptr,
			nullptr
		);
	}

	if (!window) {
		--windowCount;
		if (windowCount == 0) glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	} else {
		if (!initOpenGL(width, height)) {
			glfwDestroyWindow(window);
			--windowCount;
			if (windowCount == 0) glfwTerminate();
			throw std::runtime_error("Failed to initialize OpenGL");
		} else {
			glfwSetWindowUserPointer(window, this);
			setInternalCallbacks();
		}
	}
}

bool Window::initOpenGL(int width, int height) {
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		return false;
	}

	std::cout << "GL VERSION: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GL VENDOR: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL RENDERER: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GLSL VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glFrontFace(GL_CW);

	glfwSwapInterval(1);

	glViewport(0, 0, width, height);

	return true;
}

void Window::close() { active = false; }

Window::~Window() {
	std::cout << "Destroying window\n";

	models.clear();
	meshes.clear();
	shaders.clear();
	cameraControllers.clear();
	cameras.clear();

	if (window) glfwDestroyWindow(window);

	--windowCount;
	if (windowCount == 0) glfwTerminate();
}
