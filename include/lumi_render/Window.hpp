#pragma once

#include <unordered_map>
#include <deque>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <lumi/Shader.hpp>
#include <lumi/Geometry/Mesh.hpp>
#include <lumi/Geometry/Model.hpp>
#include <lumi/Camera/Camera.hpp>
#include <lumi/Camera/CameraController.hpp>
#include <lumi/Key.hpp>

// Window events
using LumiWindowposfun          = void(*)(Window* window, int x, int y);                                // Window moved
using LumiWindowsizefun         = void(*)(Window* window, int width, int height);                       // Window resized
using LumiWindowclosefun        = void(*)(Window* window);                                              // Window close requested
using LumiWindowrefreshfun      = void(*)(Window* window);                                              // Window needs redraw
using LumiWindowfocusfun        = void(*)(Window* window, int focused);                                 // Window gained/lost focus
using LumiWindowiconifyfun      = void(*)(Window* window, int iconified);                               // Window minimized/restored
using LumiWindowmaximizefun     = void(*)(Window* window, int maximized);                               // Window maximized/restored
using LumiFramebuffersizefun    = void(*)(Window* window, int width, int height);                       // Framebuffer resized
using LumiWindowcontentscalefun = void(*)(Window* window, float xscale, float yscale);                  // DPI/content scale changed

// Keyboard events
using LumiKeyfun                = void(*)(Window* window, int key, int scancode, int action, int mods); // Key pressed/released/repeated
using LumiCharfun               = void(*)(Window* window, unsigned int codepoint);                      // Unicode character input

// Mouse events
using LumiCursorposfun          = void(*)(Window* window, double x, double y);                          // Mouse moved
using LumiCursorenterfun        = void(*)(Window* window, int entered);                                 // Mouse entered/left window
using LumiMousebuttonfun        = void(*)(Window* window, int button, int action, int mods);            // Mouse button pressed/released
using LumiScrollfun             = void(*)(Window* window, double dX, double dY);                        // Mouse wheel scrolled

// File events
using LumiDropfun               = void(*)(Window* window, int pathCount, const char** paths);           // Files dropped onto window

struct MeshEntry {
	std::string id;
	Mesh mesh;

	MeshEntry(std::string id, Mesh mesh) : id(std::move(id)), mesh(std::move(mesh)) {}

	MeshEntry(const MeshEntry&) = delete;
	MeshEntry& operator=(const MeshEntry&) = delete;
	MeshEntry(MeshEntry&&) noexcept = default;
	MeshEntry& operator=(MeshEntry&&) noexcept = default;
};

struct ModelEntry {
	std::string id;
	Model model;

	ModelEntry(std::string id, Model model) : id(std::move(id)), model(std::move(model)) {}

	ModelEntry(const ModelEntry&) = delete;
	ModelEntry& operator=(const ModelEntry&) = delete;
	ModelEntry(ModelEntry&&) noexcept = default;
	ModelEntry& operator=(ModelEntry&&) noexcept = default;
};

class Window {
public:
	Window(int width = 854, int height = 480, bool fullscreen = false);
	~Window();

	GLFWwindow *getWindow();

	int getWidth() { return width; }
	int getHeight() { return height; }
	bool getFullscreen() { return fullscreen; }

	Shader* getShader(const std::string id);
	Camera* getCamera(const std::string id);

	Mesh* getMesh(const std::string shaderID, const std::string camID, const std::string meshID);
	Model* getModel(const std::string shaderID, const std::string camID, const std::string modelID);

	bool hasCamera(const std::string& id) const;
	void addCamera(std::string id);

	void setCameraController(const std::string& camID, std::unique_ptr<CameraController> controller);
	void clearCameraController(const std::string& camID);
	CameraController* getCameraController(const std::string& camID); // nullptr if no controller is set

	bool hasShader(const std::string& id) const;
	void addShader(std::string id, const std::string& vertexPath, const std::string& fragmentPath);

	bool isKeyDown(Key key) const;
	Vec2 getMouseDelta() const;
	void setCursorCaptured(bool captured);
	bool isCursorCaptured() const;

	void addMesh(Mesh mesh, std::string shaderID, std::string camID, std::string meshID);
	void addModel(std::string shaderID, std::string camID, std::string modelID, std::string path, bool normalizeToUnitCube = false);

	// event callback setters
	void setWindowPosCallback(LumiWindowposfun callback);
	void setWindowSizeCallback(LumiWindowsizefun callback);
	void setWindowCloseCallback(LumiWindowclosefun callback);
	void setWindowRefreshCallback(LumiWindowrefreshfun callback);
	void setWindowFocusCallback(LumiWindowfocusfun callback);
	void setWindowIconifyCallback(LumiWindowiconifyfun callback);
	void setWindowMaximizeCallback(LumiWindowmaximizefun callback);
	void setFramebufferSizeCallback(LumiFramebuffersizefun callback);
	void setWindowContentScaleCallback(LumiWindowcontentscalefun callback);
	void setKeyCallback(LumiKeyfun callback);
	void setCharCallback(LumiCharfun callback);
	void setCursorPosCallback(LumiCursorposfun callback);
	void setCursorEnterCallback(LumiCursorenterfun callback);
	void setMouseButtonCallback(LumiMousebuttonfun callback);
	void setScrollCallback(LumiScrollfun callback);
	void setDropCallback(LumiDropfun callback);

	void close();

	void loop();

private:
	GLFWwindow *window;

	static int windowCount;

	int width{};
	int height{};
	bool fullscreen{};
	bool active = false;

	std::unordered_map<std::string, Camera> cameras;
	std::unordered_map<std::string, Shader> shaders;
	std::unordered_map<std::string, std::unique_ptr<CameraController>> cameraControllers;

	double mouseX{0.0}, mouseY{0.0};
	Vec2 mouseDelta;
	bool mouseInitialized{false};

	// map<shaderID, map<CamID, deque<{objectID, object}>>>
	std::unordered_map<std::string, std::unordered_map<std::string, std::deque<MeshEntry>>> meshes;
	std::unordered_map<std::string, std::unordered_map<std::string, std::deque<ModelEntry>>> models;

	bool initOpenGL(int width, int height);
	bool checkError(const char* where);

	void setInternalCallbacks();

	// static functions to rout glfw event callbacks through
	static void staticWindowPosCallback(GLFWwindow *glfwWindow, int xpos, int ypos);
	static void staticWindowSizeCallback(GLFWwindow *glfwWindow, int width, int height);
	static void staticWindowCloseCallback(GLFWwindow *glfwWindow);
	static void staticWindowRefreshCallback(GLFWwindow *glfwWindow);
	static void staticWindowFocusCallback(GLFWwindow *glfwWindow, int focused);
	static void staticWindowIconifyCallback(GLFWwindow *glfwWindow, int iconified);
	static void staticWindowMaximizeCallback(GLFWwindow *glfwWindow, int maximized);
	static void staticFramebufferSizeCallback(GLFWwindow *glfwWindow, int width, int height);
	static void staticWindowContentScaleCallback(GLFWwindow *glfwWindow, float xscale, float yscale);
	static void staticKeyCallback(GLFWwindow *glfwWindow, int key, int scancode, int action, int mods);
	static void staticCharCallback(GLFWwindow *glfwWindow, unsigned int codepoint);
	static void staticCursorPosCallback(GLFWwindow *glfwWindow, double xpos, double ypos);
	static void staticCursorEnterCallback(GLFWwindow *glfwWindow, int entered);
	static void staticMouseButtonCallback(GLFWwindow *glfwWindow, int button, int action, int mods);
	static void staticScrollCallback(GLFWwindow *glfwWindow, double xoffset, double yoffset);
	static void staticDropCallback(GLFWwindow *glfwWindow, int path_count, const char **paths);

	// internal event functions
	void windowPosCallback(int xpos, int ypos);
	void windowSizeCallback(int width, int height);
	void windowCloseCallback();
	void windowRefreshCallback();
	void windowFocusCallback(int focused);
	void windowIconifyCallback(int iconified);
	void windowMaximizeCallback(int maximized);
	void framebufferSizeCallback(int width, int height);
	void windowContentScaleCallback(float xscale, float yscale);
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int codepoint);
	void cursorPosCallback(double xpos, double ypos);
	void cursorEnterCallback(int entered);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
	void dropCallback(int path_count, const char **paths);

	// custom event functions
	LumiWindowposfun customWindowPosCallback = nullptr;
	LumiWindowsizefun customWindowSizeCallback = nullptr;
	LumiWindowclosefun customWindowCloseCallback = nullptr;
	LumiWindowrefreshfun customWindowRefreshCallback = nullptr;
	LumiWindowfocusfun customWindowFocusCallback = nullptr;
	LumiWindowiconifyfun customWindowIconifyCallback = nullptr;
	LumiWindowmaximizefun customWindowMaximizeCallback = nullptr;
	LumiFramebuffersizefun customFramebufferSizeCallback = nullptr;
	LumiWindowcontentscalefun customWindowContentScaleCallback = nullptr;
	LumiKeyfun customKeyCallback = nullptr;
	LumiCharfun customCharCallback = nullptr;
	LumiCursorposfun customCursorPosCallback = nullptr;
	LumiCursorenterfun customCursorEnterCallback = nullptr;
	LumiMousebuttonfun customMouseButtonCallback = nullptr;
	LumiScrollfun customScrollCallback = nullptr;
	LumiDropfun customDropCallback = nullptr;
};
