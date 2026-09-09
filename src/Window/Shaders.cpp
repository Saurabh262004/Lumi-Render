#include <lumi_render/Window.hpp>

Shader* Window::getShader(const std::string id) {
	return &shaders.at(id);
}

bool Window::hasShader(const std::string& id) const {
	return shaders.find(id) != shaders.end();
}

void Window::addShaderFromFiles(std::string id, const std::string& vertexPath, const std::string& fragmentPath) {
	shaders.try_emplace(
		std::move(id),
		Shader::fromFiles(vertexPath, fragmentPath)
	);
}

void Window::addShaderFromSource(std::string id, const std::string& vertexSource, const std::string& fragmentSource) {
	shaders.try_emplace(
		std::move(id),
		Shader::fromSource(vertexSource, fragmentSource)
	);
}
