#include <stdexcept>
#include <algorithm>

#include <lumi_render/Window.hpp>

bool Window::hasCamera(const std::string& id) const {
	return cameras.find(id) != cameras.end();
}

Camera* Window::getCamera(const std::string id) { return &cameras.at(id); }

CameraController* Window::getCameraController(const std::string& camID) {
	auto it = cameraControllers.find(camID);
	return (it != cameraControllers.end()) ? it->second.get() : nullptr;
}

void Window::addCamera(std::string id) {
	cameras.insert_or_assign(std::move(id), Camera());
}

void Window::setCameraController(const std::string& camID, std::unique_ptr<CameraController> controller) {
	if (!hasCamera(camID)) {
		throw std::runtime_error("setCameraController: no camera registered with id \"" + camID + "\"");
	}

	if (controller) {
		cameraControllers[camID] = std::move(controller);
	} else {
		cameraControllers.erase(camID);
	}
}

void Window::clearCameraController(const std::string& camID) {
	cameraControllers.erase(camID);
}
