#pragma once

#include <lumi/Camera/CameraController.hpp>

class FreeFlyCameraController : public CameraController {
public:
	explicit FreeFlyCameraController(float moveSpeed = 3.0f, float mouseSensitivity = 0.0025f);
	~FreeFlyCameraController() override;

	void update(Window& window, Camera& camera, float deltaTime) override;

private:
	float moveSpeed;
	float mouseSensitivity;

	bool initialized{false};
	Window* boundWindow{nullptr};
};
