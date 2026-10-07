#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <lumi_render/Geometry/Vec.hpp>
#include <lumi_render/Geometry/Mat.hpp>
#include <lumi_render/Geometry/Vertex.hpp>
#include <lumi_render/Shader.hpp>
#include <lumi_render/Texture.hpp>

struct IndirectCmd {
	GLuint count;
	GLuint instanceCount;
	GLuint firstIndex;
	GLuint baseVertex;
	GLuint baseInstance;
};

struct InstanceData {
	Mat4 model; // position + rotation + scale
	Vec4 color;
	Vec4 boundingSphere;
};

class Mesh {
public:
	Mesh(const void* vertexData, std::size_t vertexCount, const VertexLayout& layout);
	Mesh(const void* vertexData, std::size_t vertexCount, const std::uint32_t* indices, std::size_t indexCount, const VertexLayout& layout);
	~Mesh();

	std::vector<InstanceData>* getInstaceBuffer() { return &instanceBuffer; }
	GLuint getDrawCommandBuffer() const { return drawCommandBuffer; }
	Vec4 getLocalBoundingSphere() const { return localBoundingSphere; }

	GLuint debugReadVisibleCount() const;

	void reserveInstanceSpace(std::size_t count) { instanceBuffer.reserve(count); }

	/**
	 * injects `localBoundingSphere`
	 */
	void addInstance(const Mat4& model, const Vec4& color) {
		addInstance({ model, color, localBoundingSphere });
		//std::cout << localBoundingSphere.x << ", " << localBoundingSphere.y << ", " << localBoundingSphere.z << ", " << localBoundingSphere.w << std::endl;
	}

	/**
	 * does not inject `localBoundingSphere`
     */
	void addInstance(const InstanceData& instance) { instanceBuffer.push_back(instance); }

	void addNormalInstance() { addInstance(Mat4::identity(), {1, 1, 1, 1}); }

	void clearInstanceBuffer() { instanceBuffer.clear(); }

	void setInstanceData(const InstanceData* data, std::size_t count, GLenum usage = GL_STATIC_DRAW);

	void updateInstanceData(const InstanceData* data, std::size_t count);

	void uploadInstanceBuffer(GLenum usage = GL_STATIC_DRAW) {
		setInstanceData(instanceBuffer.data(), instanceBuffer.size(), usage);
	}

	void reUploadInstanceBuffer() {
		updateInstanceData(instanceBuffer.data(), instanceBuffer.size());
	}

	void setMaterialColor(const Vec4& color) { materialColor = color; }
	void setTexture(Texture&& tex) { texture = std::move(tex); }

	void setTransparent(bool value) { transparent = value; }
	bool isTransparent() const { return transparent; }

	void ensureCullingBuffers();
	void ensureIndirectDrawBuffer();

	void draw(const Shader& shader) const;
	void drawIndirect(const Shader& drawShader, const Mat4& viewProj) const;
	void cullAndDraw(const Shader& cullShader, const Shader& drawShader, const std::array<Vec4,6>& frustumPlanes, const Mat4& viewProj);

	void bindInstanceStorageBuffer(GLuint bindingIndex) const {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, instanceVBO);
	}

	void bindVisibleIndexBuffer(GLuint bindingIndex) const {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, visibleIndexBuffer);
	}

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&& other) noexcept;
	Mesh& operator=(Mesh&& other) noexcept;

private:
	GLuint VAO{0};
	GLuint VBO{0};
	GLuint EBO{0};
	GLuint instanceVBO{0};
	GLuint visibleIndexBuffer{0};
	GLuint drawCommandBuffer{0};
	GLuint instanceBaseLocation{1};

	std::size_t vertexCount{0};
	std::size_t indexCount{0};
	std::size_t instanceCount{0};
	std::vector<InstanceData> instanceBuffer;

	Vec4 localBoundingSphere{};
	Vec4 materialColor{1.0f, 1.0f, 1.0f, 1.0f};
	std::optional<Texture> texture;	

	bool transparent{false};
};
