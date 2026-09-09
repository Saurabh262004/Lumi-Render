#pragma once
#include <string>
#include <vector>
#include <lumi_render/Geometry/Mesh.hpp>
#include <lumi_render/Shader.hpp>

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

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

	Vec4 computeTangent(aiMesh* mesh, unsigned i, const Vec3& normal) const;

	std::vector<Vertex> extractVertices(aiMesh* mesh) const;
	std::vector<std::uint32_t> extractIndices(aiMesh* mesh) const;
	Vertex extractVertex(aiMesh* mesh, unsigned i) const;

	void loadMaterialColor(Mesh& result, aiMaterial* material);
	void loadEmbeddedTexture(Mesh& result, const aiScene* scene, const std::string& pathStr);
	void loadExternalTexture(Mesh& result, std::string pathStr);
	void loadMaterial(Mesh& result, const aiScene* scene, unsigned materialIndex);
	void loadMaterialTexture(Mesh& result, const aiScene* scene, aiMaterial* material);

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
