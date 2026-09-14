#include <math.h>
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

int Window::getAverageFPS(uint32_t milliseconds) const {
    if (frameTimes.size() < 2) return 0.0f;

    milliseconds = std::min(milliseconds, 2000u);

    double currentTime = frameTimes.back();
    double startTime = currentTime - milliseconds / 1000.0;

    auto it = frameTimes.begin();

    while (it != frameTimes.end() && *it < startTime) {
        ++it;
    }

    std::size_t frameCount = frameTimes.end() - it;

    if (frameCount < 2) return 0.0f;

    double elapsed = frameTimes.back() - *it;

    if (elapsed <= 0.0) return 0.0f;

    return round((frameCount - 1) / elapsed);
}
