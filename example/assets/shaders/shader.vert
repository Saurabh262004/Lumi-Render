#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 tangent;
layout (location = 3) in vec3 vertexColor;
layout (location = 4) in vec2 texCoord;

layout (location = 5) in mat4 model;
layout (location = 9) in vec3 instanceColor;

uniform mat4 viewProjection;

out vec3 vColor;
out vec3 vNormal;
out vec2 vTexCoord;

void main() {
	gl_Position = viewProjection * model * vec4(pos, 1.0);

	vColor = vertexColor * instanceColor;

	vNormal = mat3(model) * normal;

	vTexCoord = texCoord;
}
