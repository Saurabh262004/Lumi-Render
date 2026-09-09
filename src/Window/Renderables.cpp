#include <stdexcept>

#include <lumi_render/Window.hpp>

// Mesh handlers

Mesh* Window::getMesh(const std::string shaderID, const std::string camID, const std::string meshID) {
	for (auto& meshEntry : meshes.at(shaderID).at(camID)) {
		if (meshEntry.id == meshID) return &meshEntry.mesh;
	}

	throw std::runtime_error("getMesh: no mesh registered with id: \"" + meshID + "\" under shaderID: \"" + shaderID + "\" and camID: \"" + camID + "\"");
}

void Window::addMesh(Mesh mesh, std::string shaderID, std::string camID, std::string meshID) {
	if (!hasShader(shaderID)) {
		throw std::runtime_error("addMesh: no shader registered with id \"" + shaderID + "\"");
	}

	if (!hasCamera(camID)) {
		throw std::runtime_error("addMesh: no camera registered with id \"" + camID + "\"");
	}

	meshes[shaderID][camID].emplace_back(
		std::move(meshID),
		std::move(mesh)
	);
}

// Model handlers

Model* Window::getModel(const std::string shaderID, const std::string camID, const std::string modelID) {
	for (auto& modelEntry : models.at(shaderID).at(camID)) {
		if (modelEntry.id == modelID) return &modelEntry.model;
	}

	throw std::runtime_error("getMesh: no model registered with id: \"" + modelID + "\" under shaderID: \"" + shaderID + "\" and camID: \"" + camID + "\"");
}

void Window::addModel(std::string shaderID, std::string camID, std::string modelID, std::string path, bool normalizeToUnitCube) {
	if (!hasShader(shaderID)) {
		throw std::runtime_error("addModel: no shader registered with id \"" + shaderID + "\"");
	}

	if (!hasCamera(camID)) {
		throw std::runtime_error("addModel: no camera registered with id \"" + camID + "\"");
	}

	models[shaderID][camID].emplace_back(
		std::move(modelID),
		Model(path, normalizeToUnitCube)
	);
}
