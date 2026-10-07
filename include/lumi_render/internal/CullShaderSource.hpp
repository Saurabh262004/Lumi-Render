#pragma once

namespace lumi_render::internal {

constexpr const char* cullComputeShaderSource = R"(
#version 460 core
layout(local_size_x = 64) in;

struct InstanceData {
	mat4 model;
	vec4 color;
	vec4 boundingSphere;
};

layout(std430, binding = 0) buffer InstanceBuffer { InstanceData instances[]; };
layout(std430, binding = 1) buffer VisibleIndices  { uint visible[]; };
layout(std430, binding = 2) buffer DrawCmd {
	uint indexCount, instanceCount, firstIndex, baseVertex, baseInstance;
};

uniform vec4 frustumPlanes[6];
uniform uint totalInstances;

void main() {
	uint idx = gl_GlobalInvocationID.x;
	if (idx >= totalInstances) return;

	vec3 center = (instances[idx].model * vec4(instances[idx].boundingSphere.xyz, 1.0)).xyz;

	float scaleX = length(vec3(instances[idx].model[0]));
	float scaleY = length(vec3(instances[idx].model[1]));
	float scaleZ = length(vec3(instances[idx].model[2]));
	float maxScale = max(scaleX, max(scaleY, scaleZ));

	float radius = instances[idx].boundingSphere.w * maxScale;

	for (int i = 0; i < 6; i++) {
		if (dot(frustumPlanes[i].xyz, center) + frustumPlanes[i].w + radius < 0.0) return;
	}

	uint slot = atomicAdd(instanceCount, 1);
	visible[slot] = idx;
}
)";

}
