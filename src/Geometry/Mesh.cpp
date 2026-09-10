#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <lumi_render/Geometry/Vec.hpp>
#include <lumi_render/Geometry/Vertex.hpp>
#include <lumi_render/Geometry/Mesh.hpp>

Mesh::Mesh(const void* vertexData, std::size_t vertexCount, const VertexLayout& layout) : Mesh(vertexData, vertexCount, nullptr, 0, layout) {}

Mesh::Mesh(const void* vertexData, std::size_t vertexCount, const std::uint32_t* indices, std::size_t indexCount, const VertexLayout& layout) : vertexCount{vertexCount}, indexCount{indexCount} {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * layout.stride, vertexData, GL_STATIC_DRAW);

	GLuint maxLocation = 0;
	for (const auto& attr : layout.attributes) {
		glVertexAttribPointer(attr.location, attr.componentCount, GL_FLOAT, GL_FALSE, layout.stride, (void*)attr.offset);
		glEnableVertexAttribArray(attr.location);
		maxLocation = std::max(maxLocation, attr.location + 1);
	}
	instanceBaseLocation = maxLocation;

	if (indices && indexCount > 0) {
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(std::uint32_t), indices, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::setInstanceData(const InstanceData* data, std::size_t count, GLenum usage) {
	glBindVertexArray(VAO);

	if (instanceVBO == 0) {
		glGenBuffers(1, &instanceVBO);
	}

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(InstanceData), data, usage);

	std::size_t vec4Size = sizeof(Vec4);

	for (int i = 0; i < 4; ++i) {
		GLuint loc = instanceBaseLocation + i;
		glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(offsetof(InstanceData, model) + i * vec4Size));

		glEnableVertexAttribArray(loc);
		glVertexAttribDivisor(loc, 1);
	}

	GLuint colorLoc = instanceBaseLocation + 4;

	glVertexAttribPointer(
		colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData),
		(void*)offsetof(InstanceData, color)
	);

	glEnableVertexAttribArray(colorLoc);
	glVertexAttribDivisor(colorLoc, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	instanceCount += count;
}

void Mesh::addNormalInstance() {
	InstanceData instance = { Mat4::identity(), {1, 1, 1, 1} };

	addInstance(instance);
}

void Mesh::updateInstanceData(const InstanceData* data, std::size_t count) {
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(InstanceData), data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::draw(const Shader& shader) const {
	shader.uploadVec4("material.color", materialColor);
	shader.uploadInt("material.hasTexture", texture.has_value() ? 1 : 0);

	if (texture) {
		texture->bind(0);
		shader.uploadInt("material.diffuse", 0);
	}

	glBindVertexArray(VAO);

	if (EBO != 0) {
		glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(instanceCount));
	} else {
		glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount), static_cast<GLsizei>(instanceCount));
	}

	glBindVertexArray(0);
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &instanceVBO);
}

Mesh::Mesh(Mesh&& other) noexcept :
	VAO{other.VAO}, VBO{other.VBO}, EBO{other.EBO}, instanceVBO{other.instanceVBO},
	instanceBaseLocation{other.instanceBaseLocation}, vertexCount{other.vertexCount}, indexCount{other.indexCount}, instanceCount{other.instanceCount},
	instanceBuffer{std::move(other.instanceBuffer)},
	materialColor{other.materialColor}, texture{std::move(other.texture)}
{
	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;
	other.instanceVBO = 0;
	other.instanceBaseLocation = 1;
	other.vertexCount = 0;
	other.indexCount = 0;
	other.instanceCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
	if (this != &other) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &instanceVBO);

		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;
		instanceVBO = other.instanceVBO;
		instanceBaseLocation = other.instanceBaseLocation;
		vertexCount = other.vertexCount;
		indexCount = other.indexCount;
		instanceCount = other.instanceCount;
		instanceBuffer = std::move(other.instanceBuffer);
		materialColor = other.materialColor;
		texture = std::move(other.texture);

		other.VAO = 0;
		other.VBO = 0;
		other.EBO = 0;
		other.instanceVBO = 0;
		other.instanceBaseLocation = 1;
		other.vertexCount = 0;
		other.indexCount = 0;
		other.instanceCount = 0;
	}
	return *this;
}
