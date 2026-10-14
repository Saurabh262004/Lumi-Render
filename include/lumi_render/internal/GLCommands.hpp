#pragma once
#include <glad/glad.h>

namespace lumi_render::internal {
	struct IndirectCmd { GLuint count, instanceCount, firstIndex, baseVertex, baseInstance; };

	constexpr GLuint BINDING_INSTANCES = 0;
	constexpr GLuint BINDING_INDICES   = 1;
	constexpr GLuint BINDING_DRAWCMD   = 2;
}
