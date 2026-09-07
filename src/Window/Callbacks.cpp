#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <lumi_render/Util.hpp>
#include <lumi_render/Window.hpp>

void Window::setInternalCallbacks() {
	glfwSetWindowPosCallback(window, staticWindowPosCallback);
	glfwSetWindowSizeCallback(window, staticWindowSizeCallback);
	glfwSetWindowCloseCallback(window, staticWindowCloseCallback);
	glfwSetWindowRefreshCallback(window, staticWindowRefreshCallback);
	glfwSetWindowFocusCallback(window, staticWindowFocusCallback);
	glfwSetWindowIconifyCallback(window, staticWindowIconifyCallback);
	glfwSetWindowMaximizeCallback(window, staticWindowMaximizeCallback);
	glfwSetFramebufferSizeCallback(window, staticFramebufferSizeCallback);
	glfwSetWindowContentScaleCallback(window, staticWindowContentScaleCallback);
	glfwSetKeyCallback(window, staticKeyCallback);
	glfwSetCharCallback(window, staticCharCallback);
	glfwSetCursorPosCallback(window, staticCursorPosCallback);
	glfwSetCursorEnterCallback(window, staticCursorEnterCallback);
	glfwSetMouseButtonCallback(window, staticMouseButtonCallback);
	glfwSetScrollCallback(window, staticScrollCallback);
	glfwSetDropCallback(window, staticDropCallback);
}

// static functions to rout glfw event callbacks through

void Window::staticWindowPosCallback(GLFWwindow *glfwWindow, int xpos, int ypos) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->windowPosCallback(xpos, ypos);
}

void Window::staticWindowSizeCallback(GLFWwindow *glfwWindow, int width, int height) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->windowSizeCallback(width, height);
}

void Window::staticWindowCloseCallback(GLFWwindow *glfwWindow) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->windowCloseCallback();
}

void Window::staticWindowRefreshCallback(GLFWwindow *glfwWindow) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->windowRefreshCallback();
}

void Window::staticWindowFocusCallback(GLFWwindow *glfwWindow, int focused) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->windowFocusCallback(focused);
}

void Window::staticWindowIconifyCallback(GLFWwindow *glfwWindow, int iconified) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->windowIconifyCallback(iconified);
}

void Window::staticWindowMaximizeCallback(GLFWwindow *glfwWindow, int maximized) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->windowMaximizeCallback(maximized);
}

void Window::staticFramebufferSizeCallback(GLFWwindow *glfwWindow, int width, int height) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->framebufferSizeCallback(width, height);
}

void Window::staticWindowContentScaleCallback(GLFWwindow *glfwWindow, float xscale, float yscale) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->windowContentScaleCallback(xscale, yscale);
}

void Window::staticKeyCallback(GLFWwindow *glfwWindow, int key, int scancode, int action, int mods) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->keyCallback(key, scancode, action, mods);
}

void Window::staticCharCallback(GLFWwindow *glfwWindow, unsigned int codepoint) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->charCallback(codepoint);
}

void Window::staticCursorPosCallback(GLFWwindow *glfwWindow, double xpos, double ypos) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->cursorPosCallback(xpos, ypos);
}

void Window::staticCursorEnterCallback(GLFWwindow *glfwWindow, int entered) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->cursorEnterCallback(entered);
}

void Window::staticMouseButtonCallback(GLFWwindow *glfwWindow, int button, int action, int mods) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->mouseButtonCallback(button,  action, mods);
}

void Window::staticScrollCallback(GLFWwindow *glfwWindow, double xoffset, double yoffset) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->scrollCallback(xoffset, yoffset);
}

void Window::staticDropCallback(GLFWwindow *glfwWindow, int path_count, const char **paths) {
	Window* self = static_cast<Window*>(
		glfwGetWindowUserPointer(glfwWindow)
	);

	self->dropCallback(path_count, paths);
}

// internal event functions

void Window::windowPosCallback(int xpos, int ypos) {
	if (customWindowPosCallback)
		customWindowPosCallback(this, xpos, ypos);
}

void Window::windowSizeCallback(int width, int height) {
	if (customWindowSizeCallback)
		customWindowSizeCallback(this, width, height);
}

void Window::windowCloseCallback() {
	if (customWindowCloseCallback)
		customWindowCloseCallback(this);
}

void Window::windowRefreshCallback() {
	if (customWindowRefreshCallback)
		customWindowRefreshCallback(this);
}

void Window::windowFocusCallback(int focused) {
	if (customWindowFocusCallback)
		customWindowFocusCallback(this, focused);
}

void Window::windowIconifyCallback(int iconified) {
	if (customWindowIconifyCallback)
		customWindowIconifyCallback(this, iconified);
}

void Window::windowMaximizeCallback(int maximized) {
	if (customWindowMaximizeCallback)
		customWindowMaximizeCallback(this, maximized);
}

void Window::framebufferSizeCallback(int width, int height) {
	this->width = width;
	this->height = height;

	glViewport(0, 0, width, height);

	for (auto& [camID, camera] : cameras) {
		camera.setViewportResolution((float)width, (float)height);
	}

	if (customFramebufferSizeCallback)
		customFramebufferSizeCallback(this, width, height);
}

void Window::windowContentScaleCallback(float xscale, float yscale) {
	if (customWindowContentScaleCallback)
		customWindowContentScaleCallback(this, xscale, yscale);
}

void Window::keyCallback(int key, int scancode, int action, int mods) {

	if (customKeyCallback)
		customKeyCallback(this, key, scancode, action, mods);
}

void Window::charCallback(unsigned int codepoint) {
	if (customCharCallback)
		customCharCallback(this, codepoint);
}

void Window::cursorPosCallback(double xpos, double ypos) {
	if (customCursorPosCallback)
		customCursorPosCallback(this, xpos, ypos);
}

void Window::cursorEnterCallback(int entered) {
	if (customCursorEnterCallback)
		customCursorEnterCallback(this, entered);
}

void Window::mouseButtonCallback(int button, int action, int mods) {
	if (customMouseButtonCallback)
		customMouseButtonCallback(this, button, action, mods);
}

void Window::scrollCallback(double xoffset, double yoffset) {
	if (customScrollCallback)
		customScrollCallback(this, xoffset, yoffset);
}

void Window::dropCallback(int path_count, const char **paths) {
	if (customDropCallback)
		customDropCallback(this, path_count, paths);
}

// custom event setting function

void Window::setWindowPosCallback(LumiWindowposfun callback) {
	customWindowPosCallback = callback;
}

void Window::setWindowSizeCallback(LumiWindowsizefun callback) {
	customWindowSizeCallback = callback;
}

void Window::setWindowCloseCallback(LumiWindowclosefun callback) {
	customWindowCloseCallback = callback;
}

void Window::setWindowRefreshCallback(LumiWindowrefreshfun callback) {
	customWindowRefreshCallback = callback;
}

void Window::setWindowFocusCallback(LumiWindowfocusfun callback) {
	customWindowFocusCallback = callback;
}

void Window::setWindowIconifyCallback(LumiWindowiconifyfun callback) {
	customWindowIconifyCallback = callback;
}

void Window::setWindowMaximizeCallback(LumiWindowmaximizefun callback) {
	customWindowMaximizeCallback = callback;
}

void Window::setFramebufferSizeCallback(LumiFramebuffersizefun callback) {
	customFramebufferSizeCallback = callback;
}

void Window::setWindowContentScaleCallback(LumiWindowcontentscalefun callback) {
	customWindowContentScaleCallback = callback;
}

void Window::setKeyCallback(LumiKeyfun callback) {
	customKeyCallback = callback;
}

void Window::setCharCallback(LumiCharfun callback) {
	customCharCallback = callback;
}

void Window::setCursorPosCallback(LumiCursorposfun callback) {
	customCursorPosCallback = callback;
}

void Window::setCursorEnterCallback(LumiCursorenterfun callback) {
	customCursorEnterCallback = callback;
}

void Window::setMouseButtonCallback(LumiMousebuttonfun callback) {
	customMouseButtonCallback = callback;
}

void Window::setScrollCallback(LumiScrollfun callback) {
	customScrollCallback = callback;
}

void Window::setDropCallback(LumiDropfun callback) {
	customDropCallback = callback;
}

void Window::setLumiPreLoop(LumiLoop callback) {
	lumiPreLoop = callback;
}

void Window::setLumiPostLoop(LumiLoop callback) {
	lumiPostLoop = callback;
}
