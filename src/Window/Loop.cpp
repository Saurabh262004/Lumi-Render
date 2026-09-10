#include <iostream>

#include <lumi_render/Shader.hpp>
#include <lumi_render/Window.hpp>

void Window::loop() {
	bool errorsInLoop = false;

	double lastTime = glfwGetTime();

	active = true;
	while (active && !glfwWindowShouldClose(window) && !errorsInLoop) {
		if (lumiPreLoop) lumiPreLoop(this);

		double currentTime = glfwGetTime();
		float deltaTime = static_cast<float>(currentTime - lastTime);
		fps = 1.0f / deltaTime;
		lastTime = currentTime;

		if (clearColorBuffer || clearDepthBuffer) {
			GLbitfield clearMask = 0;

			if (clearColorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;

			if (clearDepthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;

			glClearColor(
				clearColor.x,
				clearColor.y,
				clearColor.z,
				clearColor.w
			);

			glClear(clearMask);
		}

		double currentMouseX, currentMouseY;
		glfwGetCursorPos(window, &currentMouseX, &currentMouseY);

		if (!mouseInitialized) {
			mouseX = currentMouseX;
			mouseY = currentMouseY;
			mouseInitialized = true;
		}

		mouseDelta = Vec2{static_cast<float>(currentMouseX - mouseX), static_cast<float>(currentMouseY - mouseY)};
		mouseX = currentMouseX;
		mouseY = currentMouseY;

		for (auto& [camID, camera] : cameras) {
			if (CameraController* controller = getCameraController(camID)) {
				controller->update(*this, camera, deltaTime);
			}

			camera.updateViewProjection();
		}

		// TEMPORARY FIX

		// opaque meshes
		for (auto& [shaderID, camMap] : meshes) {
			shaders.at(shaderID).use();

			if (checkError("mesh, shader use")) errorsInLoop = true;

			shaders.at(shaderID).uploadUniforms();

			if (checkError("mesh, shader upload uniforms")) errorsInLoop = true;

			for (auto& [camID, meshEntries] : camMap) {
				shaders.at(shaderID).uploadMat4("viewProjection", cameras.at(camID).getViewProjection());

				if (checkError("mesh, shader set viewProjection")) errorsInLoop = true;

				for (auto& meshEntry : meshEntries) {
					if (!meshEntry.mesh.isTransparent()) meshEntry.mesh.draw(shaders.at(shaderID));
				}

				if (checkError("mesh, draw")) errorsInLoop = true;
			}
		}

		// opaque model meshes
		for (auto& [shaderID, camMap] : models) {
			shaders.at(shaderID).use();

			if (checkError("model, shader use")) errorsInLoop = true;

			shaders.at(shaderID).uploadUniforms();

			if (checkError("model, shader upload uniforms")) errorsInLoop = true;

			for (auto& [camID, modelEntries] : camMap) {
				shaders.at(shaderID).uploadMat4("viewProjection", cameras.at(camID).getViewProjection());

				if (checkError("model, shader set viewProjection")) errorsInLoop = true;

				for (auto& modelEntry : modelEntries) modelEntry.model.drawOpaque(shaders.at(shaderID));

				if (checkError("model, draw")) errorsInLoop = true;
			}
		}

		// transparent meshes
		for (auto& [shaderID, camMap] : meshes) {
			shaders.at(shaderID).use();

			if (checkError("mesh, shader use")) errorsInLoop = true;

			shaders.at(shaderID).uploadUniforms();

			if (checkError("mesh, shader upload uniforms")) errorsInLoop = true;

			for (auto& [camID, meshEntries] : camMap) {
				shaders.at(shaderID).uploadMat4("viewProjection", cameras.at(camID).getViewProjection());

				if (checkError("mesh, shader set viewProjection")) errorsInLoop = true;

				for (auto& meshEntry : meshEntries) {
					if (meshEntry.mesh.isTransparent()) meshEntry.mesh.draw(shaders.at(shaderID));
				}

				if (checkError("mesh, draw")) errorsInLoop = true;
			}
		}

		// transparent model meshes
		for (auto& [shaderID, camMap] : models) {
			shaders.at(shaderID).use();

			if (checkError("model, shader use")) errorsInLoop = true;

			shaders.at(shaderID).uploadUniforms();

			if (checkError("model, shader upload uniforms")) errorsInLoop = true;

			for (auto& [camID, modelEntries] : camMap) {
				shaders.at(shaderID).uploadMat4("viewProjection", cameras.at(camID).getViewProjection());

				if (checkError("model, shader set viewProjection")) errorsInLoop = true;

				for (auto& modelEntry : modelEntries) modelEntry.model.drawTransparent(shaders.at(shaderID));

				if (checkError("model, draw")) errorsInLoop = true;
			}
		}

		checkError("Main Loop");

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (lumiPostLoop) lumiPostLoop(this);
	}
}
