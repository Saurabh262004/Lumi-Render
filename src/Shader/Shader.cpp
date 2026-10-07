#include <iostream>
#include <utility>
#include <algorithm>
#include <stdexcept>

#include <lumi_render/Shader.hpp>
#include <lumi_render/Util.hpp>

Shader Shader::fromSource(const std::string& vertexSource, const std::string& fragmentSource) {
	return Shader(vertexSource, fragmentSource);
}

Shader Shader::fromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
	std::string vertexCode = Util::readFile(vertexPath);
	const char* vertexSource = vertexCode.c_str();

	std::string fragmentCode = Util::readFile(fragmentPath);
	const char* fragmentSource = fragmentCode.c_str();

	return Shader(vertexSource, fragmentSource);
}

Shader Shader::fromComputeSource(const std::string& computeSource) {
	return Shader(computeSource);
}

Shader Shader::fromComputeFile(const std::string& computePath) {
	std::string computeCode = Util::readFile(computePath);
	return Shader(computeCode);
}

Shader Shader::fromCulledFiles(const std::string& vertexPath, const std::string& fragmentPath) {
	std::string vertexSource = Util::readFile(vertexPath);
	std::string fragmentSource = Util::readFile(fragmentPath);
	return Shader(injectInstanceCulling(vertexSource), fragmentSource);
}

Shader Shader::fromCulledSource(const std::string& vertexSource, const std::string& fragmentSource) {
	return Shader(injectInstanceCulling(vertexSource), fragmentSource);
}

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource) {
	GLint success;

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexSourceCStr = vertexSource.c_str();
	glShaderSource(vertexShader, 1, &vertexSourceCStr, nullptr);

	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(vertexShader, 1024, nullptr, infoLog);

		glDeleteShader(vertexShader);

		throw std::runtime_error(
			std::string("Vertex shader compilation failed:\n") + infoLog
		);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentSourceCStr = fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &fragmentSourceCStr, nullptr);

	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(fragmentShader, 1024, nullptr, infoLog);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		throw std::runtime_error(
			std::string("Fragment shader compilation failed:\n") + infoLog
		);
	}

	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success) {
		char infoLog[1024];

		glGetProgramInfoLog(program, 1024, nullptr, infoLog);

		glDeleteProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		throw std::runtime_error(
			std::string("Shader program linking failed:\n") + infoLog
		);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(const std::string& computeSource) {
	GLint success;

	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	const char* srcCStr = computeSource.c_str();
	glShaderSource(computeShader, 1, &srcCStr, nullptr);
	glCompileShader(computeShader);

	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[1024];

		glGetShaderInfoLog(computeShader, 1024, nullptr, infoLog);
		glDeleteShader(computeShader);

		throw std::runtime_error(
			std::string("Compute shader compilation failed:\n") + infoLog
		);
	}

	program = glCreateProgram();
	glAttachShader(program, computeShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success) {
		char infoLog[1024];

		glGetProgramInfoLog(program, 1024, nullptr, infoLog);

		glDeleteProgram(program);
		glDeleteShader(computeShader);

		throw std::runtime_error(
			std::string("Compute program linking failed:\n") + infoLog
		);
	}

	glDeleteShader(computeShader);
}

std::string Shader::injectInstanceCulling(const std::string& vertexSource) {
	std::string src = vertexSource;

	auto stripLine = [&](const std::string& needle) {
		auto pos = src.find(needle);

		if (pos == std::string::npos) return;

		auto lineStart = src.rfind('\n', pos) + 1;
		auto lineEnd = src.find('\n', pos);

		lineEnd = (lineEnd == std::string::npos) ? src.size() : lineEnd + 1;
		src.erase(lineStart, lineEnd - lineStart);
	};

	stripLine("in mat4 model;");
	stripLine("in vec4 instanceColor;");

	std::string ssboDecl =
		"\nstruct InstanceData { mat4 model; vec4 color; vec4 boundingSphere; };\n"
		"layout(std430, binding = 0) buffer InstanceBuffer { InstanceData instances[]; };\n"
		"layout(std430, binding = 1) buffer VisibleIndices  { uint visible[]; };\n";

	auto versionEnd = src.find('\n', src.find("#version")) + 1;

	src.insert(versionEnd, ssboDecl);

	std::string fetch =
		"\n\tuint realIdx = visible[gl_InstanceID];\n"
		"\tmat4 model = instances[realIdx].model;\n"
		"\tvec4 instanceColor = instances[realIdx].color;\n";

	auto mainPos = src.find("void main(");
	auto brace = src.find('{', mainPos);

	src.insert(brace + 1, fetch);

	return src;
}

Shader::~Shader() {
	std::cout << "Deleting shader\n";
	glDeleteProgram(program);
}

GLuint Shader::getProgram() const { return program; }

void Shader::use() const {
	glUseProgram(program);
}

Shader::Shader(Shader&& other) noexcept :
	program{std::exchange(other.program, 0)},
	vec2Uniforms{std::move(other.vec2Uniforms)},
	vec3Uniforms{std::move(other.vec3Uniforms)},
	vec4Uniforms{std::move(other.vec4Uniforms)},
	mat4Uniforms{std::move(other.mat4Uniforms)},
	intUniforms{std::move(other.intUniforms)},
	activeVec2Uniforms{std::move(other.activeVec2Uniforms)},
	activeVec3Uniforms{std::move(other.activeVec3Uniforms)},
	activeVec4Uniforms{std::move(other.activeVec4Uniforms)},
	activeMat4Uniforms{std::move(other.activeMat4Uniforms)},
	activeIntUniforms{std::move(other.activeIntUniforms)}
{}

Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		if (program != 0) glDeleteProgram(program);

		program = std::exchange(other.program, 0);
		vec2Uniforms = std::move(other.vec2Uniforms);
		vec3Uniforms = std::move(other.vec3Uniforms);
		vec4Uniforms = std::move(other.vec4Uniforms);
		mat4Uniforms = std::move(other.mat4Uniforms);
		intUniforms = std::move(other.intUniforms);
		activeVec2Uniforms = std::move(other.activeVec2Uniforms);
		activeVec3Uniforms = std::move(other.activeVec3Uniforms);
		activeVec4Uniforms = std::move(other.activeVec4Uniforms);
		activeMat4Uniforms = std::move(other.activeMat4Uniforms);
		activeIntUniforms = std::move(other.activeIntUniforms);
	}

	return *this;
}
