#include <math.h>
#include <iostream>

#include <lumi_render/Shader.hpp>
#include <lumi_render/Window.hpp>

void Window::loop() {
	bool errorsInLoop = false;

	active = true;

	double lastTime = glfwGetTime();
	while (active && !glfwWindowShouldClose(window) && !errorsInLoop) {
		if (lumiPreLoop) lumiPreLoop(this);

		double currentTime = glfwGetTime();

		float deltaTime = static_cast<float>(currentTime - lastTime);
		fps = 1.0f / deltaTime;
		lastTime = currentTime;

		frameTimes.push_back(currentTime);

		double cutoff = currentTime - maxFrameTimeHistory;

		while (!frameTimes.empty() && frameTimes.front() < cutoff) {
			frameTimes.pop_front();
		}

		if (clearColorBuffer || clearDepthBuffer) {
			GLbitfield clearMask = 0;

			if (clearColorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
			if (clearDepthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;

			clearColorBufer();

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
					if (meshEntry.useCulling) continue;
					if (!meshEntry.mesh.isTransparent()) meshEntry.mesh.draw(shaders.at(shaderID));
				}

				if (checkError("mesh, draw")) errorsInLoop = true;
			}
		}

		// opaque culled meshes
		for (auto& [shaderID, camMap] : meshes) {
			for (auto& [camID, meshEntries] : camMap) {
				auto frustumPlanes = cameras.at(camID).getFrustumPlanes();
				Mat4 viewProj = cameras.at(camID).getViewProjection();

				for (auto& meshEntry : meshEntries) {
					if (!meshEntry.useCulling || meshEntry.mesh.isTransparent()) continue;

					meshEntry.mesh.cullAndDraw(shaders.at("cull"), shaders.at(shaderID), frustumPlanes, viewProj);

					if (checkError("mesh, cullAndDraw opaque")) errorsInLoop = true;
				}
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

				for (auto& modelEntry : modelEntries) {
					if (modelEntry.useCulling) continue;
					modelEntry.model.drawOpaque(shaders.at(shaderID));
				}

				if (checkError("model, draw")) errorsInLoop = true;
			}
		}

		// opaque culled models
		for (auto& [shaderID, camMap] : models) {
			for (auto& [camID, modelEntries] : camMap) {
				auto frustumPlanes = cameras.at(camID).getFrustumPlanes();
				Mat4 viewProj = cameras.at(camID).getViewProjection();

				for (auto& modelEntry : modelEntries) {
					if (!modelEntry.useCulling) continue;

					modelEntry.model.cullCompute(shaders.at("cull"), frustumPlanes);
					modelEntry.model.drawCulledOpaque(shaders.at(shaderID), viewProj);

					if (checkError("model, cullAndDraw opaque")) errorsInLoop = true;
				}
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
					if (meshEntry.useCulling) continue;
					if (meshEntry.mesh.isTransparent()) meshEntry.mesh.draw(shaders.at(shaderID));
				}

				if (checkError("mesh, draw")) errorsInLoop = true;
			}
		}

		// transparent culled meshes
		for (auto& [shaderID, camMap] : meshes) {
			for (auto& [camID, meshEntries] : camMap) {
				auto frustumPlanes = cameras.at(camID).getFrustumPlanes();
				Mat4 viewProj = cameras.at(camID).getViewProjection();

				for (auto& meshEntry : meshEntries) {
					if (!meshEntry.useCulling || !meshEntry.mesh.isTransparent()) continue;

					meshEntry.mesh.cullAndDraw(shaders.at("cull"), shaders.at(shaderID), frustumPlanes, viewProj);

					if (checkError("mesh, cullAndDraw transparent")) errorsInLoop = true;
				}
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

				for (auto& modelEntry : modelEntries) {
					if (modelEntry.useCulling) continue;
					modelEntry.model.drawTransparent(shaders.at(shaderID));
				}

				if (checkError("model, draw")) errorsInLoop = true;
			}
		}

		// transparent culled models
		for (auto& [shaderID, camMap] : models) {
			for (auto& [camID, modelEntries] : camMap) {
				Mat4 viewProj = cameras.at(camID).getViewProjection();

				for (auto& modelEntry : modelEntries) {
					if (!modelEntry.useCulling) continue;

					modelEntry.model.drawCulledTransparent(shaders.at(shaderID), viewProj);

					if (checkError("model, drawCulledTransparent")) errorsInLoop = true;
				}
			}
		}

		checkError("Main Loop");

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (lumiPostLoop) lumiPostLoop(this);
	}
}
