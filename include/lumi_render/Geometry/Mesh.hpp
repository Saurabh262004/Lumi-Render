#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <lumi/Geometry/Vec.hpp>
#include <lumi/Geometry/Mat.hpp>
#include <lumi/Geometry/Vertex.hpp>
#include <lumi/Shader.hpp>
#include <lumi/Texture.hpp>

struct InstanceData {
	Mat4 model; // position + rotation + scale
	Vec3 color;
};

class Mesh {
public:
	Mesh(const void* vertexData, std::size_t vertexCount, const VertexLayout& layout);
	Mesh(const void* vertexData, std::size_t vertexCount, const std::uint32_t* indices, std::size_t indexCount, const VertexLayout& layout);
	~Mesh();

	void addInstance(const InstanceData& instance) { instanceBuffer.push_back(instance); }
	void reserveInstances(std::size_t count) { instanceBuffer.reserve(count); }
	void clearInstances() { instanceBuffer.clear(); }
	void uploadInstances(GLenum usage = GL_STATIC_DRAW) { setInstanceData(instanceBuffer.data(), instanceBuffer.size(), usage); }
	void addNormalInstance();

	void setInstanceData(const InstanceData* data, std::size_t count, GLenum usage = GL_STATIC_DRAW);
	void updateInstanceData(const InstanceData* data, std::size_t count);

	void setMaterialColor(const Vec3& color) { materialColor = color; }
	void setTexture(Texture&& tex) { texture = std::move(tex); }

	void draw(const Shader& shader) const;

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&& other) noexcept;
	Mesh& operator=(Mesh&& other) noexcept;

private:
	GLuint VAO{0};
	GLuint VBO{0};
	GLuint EBO{0};
	GLuint instanceVBO{0};
	GLuint instanceBaseLocation{1};
	std::size_t vertexCount{0};
	std::size_t indexCount{0};
	std::size_t instanceCount{0};
	std::vector<InstanceData> instanceBuffer;

	Vec3 materialColor{1.0f, 1.0f, 1.0f};
	std::optional<Texture> texture;
};
