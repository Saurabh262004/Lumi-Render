#include <iostream>
#include <algorithm>
#include <cfloat>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <lumi_render/Geometry/Vec.hpp>
#include <lumi_render/Geometry/Vertex.hpp>
#include <lumi_render/Geometry/Mesh.hpp>

Mesh::Mesh(const void* vertexData, std::size_t vertexCount, const VertexLayout& layout) : Mesh(vertexData, vertexCount, nullptr, 0, layout) {}

Mesh::Mesh(const void* vertexData, std::size_t vertexCount, const std::uint32_t* indices, std::size_t indexCount, const VertexLayout& layout) : vertexCount{vertexCount}, indexCount{indexCount} {
	std::size_t posOffset = 0;
	for (const auto& attr : layout.attributes) {
		if (attr.location == 0) { posOffset = attr.offset; break; } // location 0 = position, by your own convention
	}

	const char* base = static_cast<const char*>(vertexData);
	Vec3 minP{ FLT_MAX, FLT_MAX, FLT_MAX };
	Vec3 maxP{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (std::size_t i = 0; i < vertexCount; ++i) {
		const float* p = reinterpret_cast<const float*>(base + i * layout.stride + posOffset);
		Vec3 pos{ p[0], p[1], p[2] };
		minP.x = std::min(minP.x, pos.x); minP.y = std::min(minP.y, pos.y); minP.z = std::min(minP.z, pos.z);
		maxP.x = std::max(maxP.x, pos.x); maxP.y = std::max(maxP.y, pos.y); maxP.z = std::max(maxP.z, pos.z);
	}

	Vec3 center = (minP + maxP) * 0.5f;
	float radius = 0.0f;
	for (std::size_t i = 0; i < vertexCount; ++i) {
		const float* p = reinterpret_cast<const float*>(base + i * layout.stride + posOffset);
		Vec3 pos{ p[0], p[1], p[2] };
		radius = std::max(radius, (pos - center).length());
	}

	localBoundingSphere = Vec4{ center.x, center.y, center.z, radius };

	glGenVertexArrays(1, &VAO);

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

	instanceCount = count;
}

void Mesh::updateInstanceData(const InstanceData* data, std::size_t count) {
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(InstanceData), data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	instanceCount = count;
}

void Mesh::ensureIndirectDrawBuffer() {
	IndirectCmd cmd{ static_cast<GLuint>(indexCount), 0, 0, 0, 0 };
	if (drawCommandBuffer == 0) glGenBuffers(1, &drawCommandBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(cmd), &cmd, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void Mesh::ensureCullingBuffers() {
	if (visibleIndexBuffer == 0) glGenBuffers(1, &visibleIndexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleIndexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, instanceBuffer.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	ensureIndirectDrawBuffer();
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

void Mesh::drawIndirect(const Shader& drawShader, const Mat4& viewProj) const {
	drawShader.use();
	drawShader.uploadMat4("viewProjection", viewProj);
	drawShader.uploadVec4("material.color", materialColor);
	drawShader.uploadInt("material.hasTexture", texture.has_value() ? 1 : 0);
	if (texture) {
		texture->bind(0);
		drawShader.uploadInt("material.diffuse", 0);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);
	glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Mesh::cullAndDraw(const Shader& cullShader, const Shader& drawShader, const std::array<Vec4,6>& frustumPlanes, const Mat4& viewProj) {
	GLuint zero = 0;
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);
	glBufferSubData(GL_DRAW_INDIRECT_BUFFER, offsetof(IndirectCmd, instanceCount), sizeof(GLuint), &zero);

	cullShader.use();
	bindInstanceStorageBuffer(0);
	bindVisibleIndexBuffer(1);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, drawCommandBuffer);
	glUniform4fv(glGetUniformLocation(cullShader.getProgram(), "frustumPlanes"), 6, &frustumPlanes[0].x);
	glUniform1ui(glGetUniformLocation(cullShader.getProgram(), "totalInstances"), static_cast<GLuint>(instanceBuffer.size()));

	glDispatchCompute((static_cast<GLuint>(instanceBuffer.size()) + 63) / 64, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

	bindInstanceStorageBuffer(0);
	bindVisibleIndexBuffer(1);
	drawIndirect(drawShader, viewProj);
}

GLuint Mesh::debugReadVisibleCount() const {
	GLuint count = 0;
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);
	glGetBufferSubData(GL_DRAW_INDIRECT_BUFFER, offsetof(IndirectCmd, instanceCount), sizeof(GLuint), &count);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	return count;
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
	materialColor{other.materialColor}, texture{std::move(other.texture)},
	localBoundingSphere{other.localBoundingSphere},
	visibleIndexBuffer{other.visibleIndexBuffer}, drawCommandBuffer{other.drawCommandBuffer}
{
	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;
	other.instanceVBO = 0;
	other.instanceBaseLocation = 1;
	other.vertexCount = 0;
	other.indexCount = 0;
	other.instanceCount = 0;
	other.visibleIndexBuffer = 0;
	other.drawCommandBuffer = 0;
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
		localBoundingSphere = other.localBoundingSphere;

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
