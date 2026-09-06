#pragma once

#include <lumi/Camera/Camera.hpp>

class Window; // forward declaration to avoid circular include with Window.hpp

class CameraController {
public:
	virtual ~CameraController() = default;

	virtual void update(Window& window, Camera& camera, float deltaTime) = 0;
};
