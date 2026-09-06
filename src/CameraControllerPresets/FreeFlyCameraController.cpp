#include <lumi_render/Camera/FreeFlyCameraController.hpp>
#include <lumi_render/Window.hpp>
#include <lumi_render/Key.hpp>

FreeFlyCameraController::FreeFlyCameraController(float moveSpeed, float mouseSensitivity) : moveSpeed{moveSpeed}, mouseSensitivity{mouseSensitivity} {}

FreeFlyCameraController::~FreeFlyCameraController() {
	if (boundWindow) {
		boundWindow->setCursorCaptured(false);
	}
}

void FreeFlyCameraController::update(Window& window, Camera& camera, float deltaTime) {
	if (!initialized) {
		window.setCursorCaptured(true);
		boundWindow = &window;
		initialized = true;
	}

	bool leftAltHold = window.isKeyDown(Key::LeftAlt);

	if (window.isCursorCaptured()) {
		Vec2 mouseDelta = window.getMouseDelta();

		camera.setYaw(camera.getYaw() - mouseDelta.x * mouseSensitivity);
		camera.setPitch(camera.getPitch() - mouseDelta.y * mouseSensitivity);

		Vec3 forward = camera.getForward();
		Vec3 right = camera.getRight();
		Vec3 pos = camera.getPosition();

		float distance = moveSpeed * deltaTime;

		if (window.isKeyDown(Key::LeftControl)) distance *= 0.1;

		if (window.isKeyDown(Key::W))         pos   += forward * distance;
		if (window.isKeyDown(Key::S))         pos   -= forward * distance;
		if (window.isKeyDown(Key::D))         pos   += right * distance;
		if (window.isKeyDown(Key::A))         pos   -= right * distance;

		if (window.isKeyDown(Key::Space))     pos.y += distance;
		if (window.isKeyDown(Key::LeftShift)) pos.y -= distance;

		camera.setPosition(pos);

		if (leftAltHold) window.setCursorCaptured(false);
	} else if (!leftAltHold) {
		window.setCursorCaptured(true);
	}
}
