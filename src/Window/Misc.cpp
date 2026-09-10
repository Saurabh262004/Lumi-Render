#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <lumi_render/Window.hpp>

bool Window::checkError(const char* where) {
	GLenum err = glGetError();

	if (err != GL_NO_ERROR) {
		std::cout << where << " : " << err << '\n';
		return true;
	}

	return false;
}

void Window::clearColorBufer() const {
	glClearColor(
		clearColor.x,
		clearColor.y,
		clearColor.z,
		clearColor.w
	);
}
