#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <vector>

#include <lumi_render/Geometry/Vec.hpp>

struct VertexAttribute {
	GLuint location;
	GLuint componentCount;
	std::size_t offset;
};

struct VertexLayout {
	std::size_t stride;
	std::vector<VertexAttribute> attributes;
};

struct Vertex {
	Vec3 position;
	Vec3 normal;
	Vec4 tangent;
	Vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
	Vec2 texCoord{0.0f, 0.0f};

	static VertexLayout layout() {
		return VertexLayout{
			sizeof(Vertex),
			{
				{ 0, 3, offsetof(Vertex, position) },
				{ 1, 3, offsetof(Vertex, normal)   },
				{ 2, 4, offsetof(Vertex, tangent)  },
				{ 3, 4, offsetof(Vertex, color)    },
				{ 4, 2, offsetof(Vertex, texCoord) },
			}
		};
	}
};
