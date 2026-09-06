#include <stdexcept>
#include <iostream>
#include <memory>

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
	//glFrontFace(GL_CW);

	glfwSwapInterval(1);

	glViewport(0, 0, width, height);

	return true;
}

bool Window::checkError(const char* where) {
	GLenum err = glGetError();

	if (err != GL_NO_ERROR) {
		std::cout << where << " : " << err << '\n';
		return true;
	}

	return false;
}

Shader* Window::getShader(const std::string id) {
	return &shaders.at(id);
}

Mesh* Window::getMesh(const std::string shaderID, const std::string camID, const std::string meshID) {
	for (auto& meshEntry : meshes.at(shaderID).at(camID)) {
		if (meshEntry.id == meshID) return &meshEntry.mesh;
	}

	throw std::runtime_error("getMesh: no mesh registered with id: \"" + meshID + "\" under shaderID: \"" + shaderID + "\" and camID: \"" + camID + "\"");
}

Model* Window::getModel(const std::string shaderID, const std::string camID, const std::string modelID) {
	for (auto& modelEntry : models.at(shaderID).at(camID)) {
		if (modelEntry.id == modelID) return &modelEntry.model;
	}

	throw std::runtime_error("getMesh: no model registered with id: \"" + modelID + "\" under shaderID: \"" + shaderID + "\" and camID: \"" + camID + "\"");
}

bool Window::hasShader(const std::string& id) const {
	return shaders.find(id) != shaders.end();
}

bool Window::isKeyDown(Key key) const {
	return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
}

Vec2 Window::getMouseDelta() const {
	return mouseDelta;
}

void Window::setCursorCaptured(bool captured) {
	glfwSetInputMode(window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool Window::isCursorCaptured() const {
	return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void Window::addShader(std::string id, const std::string& vertexPath, const std::string& fragmentPath) {
	shaders.try_emplace(
		std::move(id),
		vertexPath,
		fragmentPath
	);
}

void Window::addMesh(Mesh mesh, std::string shaderID, std::string camID, std::string meshID) {
	if (!hasShader(shaderID)) {
		throw std::runtime_error("addMesh: no shader registered with id \"" + shaderID + "\"");
	}

	if (!hasCamera(camID)) {
		throw std::runtime_error("addMesh: no camera registered with id \"" + camID + "\"");
	}

	meshes[shaderID][camID].emplace_back(
		std::move(meshID),
		std::move(mesh)
	);
}

void Window::addModel(std::string shaderID, std::string camID, std::string modelID, std::string path, bool normalizeToUnitCube) {
	if (!hasShader(shaderID)) {
		throw std::runtime_error("addModel: no shader registered with id \"" + shaderID + "\"");
	}

	if (!hasCamera(camID)) {
		throw std::runtime_error("addModel: no camera registered with id \"" + camID + "\"");
	}

	models[shaderID][camID].emplace_back(
		std::move(modelID),
		Model(path, normalizeToUnitCube)
	);
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

GLFWwindow *Window::getWindow() {
	return window;
}
