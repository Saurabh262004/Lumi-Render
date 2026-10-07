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
	~Model();

	std::vector<InstanceData>* getInstanceBuffer() { return &instanceBuffer; }
	std::vector<Mesh>* getMeshes() { return &meshes; }
	float getAppliedScale() const { return importScale; }

	GLuint debugReadVisibleCount() const;

	void reserveInstanceSpace(std::size_t count) { instanceBuffer.reserve(count); }

	/**
	 * injects `localBoundingSphere`
	 */
	void addInstance(const Mat4& model, const Vec4& color) {
		addInstance({ model, color, localBoundingSphere });
	}

	/**
	 * does not inject `localBoundingSphere`
	 */
	void addInstance(const InstanceData& instance) { instanceBuffer.push_back(instance); }

	void addNormalInstance() { addInstance(Mat4::identity(), {1, 1, 1, 1}); }

	void clearInstanceBuffer() { instanceBuffer.clear(); }
	
	void setInstanceData(const InstanceData* data, std::size_t count, GLenum usage = GL_STATIC_DRAW) {
		for (auto& mesh : meshes) mesh.setInstanceData(data, count, usage);
	}

	void updateInstanceData(const InstanceData* data, std::size_t count) {
		for (auto& mesh : meshes) mesh.updateInstanceData(data, count);
	}

	void uploadInstanceBuffer(GLenum usage = GL_STATIC_DRAW) {
		setInstanceData(instanceBuffer.data(), instanceBuffer.size(), usage);
	}	

	void reUploadInstanceBuffer() {
		updateInstanceData(instanceBuffer.data(), instanceBuffer.size());
	}

	void drawOpaque(const Shader& shader) const {
		for (const auto& mesh : meshes) if (!mesh.isTransparent()) mesh.draw(shader);
	}

	void drawTransparent(const Shader& shader) const {
		for (const auto& mesh : meshes) if (mesh.isTransparent()) mesh.draw(shader);
	}

	Vec4 getLocalBoundingSphere() const { return localBoundingSphere; }

	void ensureCullingBuffers();

	void cullAndDraw(const Shader& cullShader, const Shader& drawShader, const std::array<Vec4,6>& frustumPlanes, const Mat4& viewProj);

	void cullCompute(const Shader& cullShader, const std::array<Vec4,6>& frustumPlanes);
	void drawCulledOpaque(const Shader& drawShader, const Mat4& viewProj) const;
	void drawCulledTransparent(const Shader& drawShader, const Mat4& viewProj) const;

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;
	Model(Model&& other) noexcept;
	Model& operator=(Model&& other) noexcept;

private:
	std::vector<Mesh> meshes;
	std::string directory;
	float importScale{1.0f};
	std::vector<InstanceData> instanceBuffer;

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

	Vec4 localBoundingSphere{};
	GLuint instanceSSBO{0};
	GLuint visibleIndexBuffer{0};
	GLuint drawCommandBuffer{0};
};
