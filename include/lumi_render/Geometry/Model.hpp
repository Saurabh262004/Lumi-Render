// Model.hpp
#pragma once
#include <string>
#include <vector>
#include <lumi/Geometry/Mesh.hpp>
#include <lumi/Shader.hpp>

struct aiNode;
struct aiScene;
struct aiMesh;

class Model {
public:
	explicit Model(const std::string& path, bool normalizeToUnitCube = false);

	float appliedScale() const { return importScale; }

	void addInstance(const InstanceData& instance) {
		for (auto& mesh : meshes) mesh.addInstance(instance);
	}

	void reserveInstances(std::size_t count) {
		for (auto& mesh : meshes) mesh.reserveInstances(count);
	}

	void clearInstances() {
		for (auto& mesh : meshes) mesh.clearInstances();
	}

	void uploadInstances(GLenum usage = GL_STATIC_DRAW) {
		for (auto& mesh : meshes) mesh.uploadInstances(usage);
	}

	void addNormalInstance() {
		for (auto& mesh : meshes) mesh.addNormalInstance();
	}

	void setInstanceData(const InstanceData* data, std::size_t count, GLenum usage = GL_STATIC_DRAW) {
		for (auto& mesh : meshes) mesh.setInstanceData(data, count, usage);
	}

	void draw(const Shader& shader) const {
		for (const auto& mesh : meshes) mesh.draw(shader);
	}

	void updateInstanceData(const InstanceData* data, std::size_t count) {
		for (auto& mesh : meshes) mesh.updateInstanceData(data, count);
	}

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;
	Model(Model&&) noexcept = default;
	Model& operator=(Model&&) noexcept = default;

private:
	std::vector<Mesh> meshes;
	std::string directory;
	float importScale{1.0f};

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
