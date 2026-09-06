#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glad/glad.h>

#include <lumi/Geometry/Vec.hpp>
#include <lumi/Geometry/Mat.hpp>

struct Vec2UniformEntry {
	Vec2 vec;
	GLint location;
};

struct Vec3UniformEntry {
	Vec3 vec;
	GLint location;
};

struct Vec4UniformEntry {
	Vec4 vec;
	GLint location;
};

struct Mat4UniformEntry {
	Mat4 mat;
	GLint location;
};

struct IntUniformEntry {
	int value;
	GLint location;
};

class Shader {
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);

	void addVec2Uniform(const std::string& id, const Vec2& vec);
	void addVec3Uniform(const std::string& id, const Vec3& vec);
	void addVec4Uniform(const std::string& id, const Vec4& vec);
	void addMat4Uniform(const std::string& id, const Mat4& mat);
	void addIntUniform(const std::string& id, int value);

	bool isVec2Uniform(const std::string& id) const;
	bool isVec3Uniform(const std::string& id) const;
	bool isVec4Uniform(const std::string& id) const;
	bool isMat4Uniform(const std::string& id) const;
	bool isIntUniform(const std::string& id) const;

	bool isVec2UniformActive(const std::string& id) const;
	bool isVec3UniformActive(const std::string& id) const;
	bool isVec4UniformActive(const std::string& id) const;
	bool isMat4UniformActive(const std::string& id) const;
	bool isIntUniformActive(const std::string& id) const;

	void activateVec2Uniform(const std::string& id);
	void activateVec3Uniform(const std::string& id);
	void activateVec4Uniform(const std::string& id);
	void activateMat4Uniform(const std::string& id);
	void activateIntUniform(const std::string& id);

	void deactivateVec2Uniform(const std::string& id);
	void deactivateVec3Uniform(const std::string& id);
	void deactivateVec4Uniform(const std::string& id);
	void deactivateMat4Uniform(const std::string& id);
	void deactivateIntUniform(const std::string& id);

	GLuint getProgram();

	~Shader();

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	void uploadVec2(const Vec2& vec, const GLint& location) const;
	void uploadVec3(const Vec3& vec, const GLint& location) const;
	void uploadVec4(const Vec4& vec, const GLint& location) const;
	void uploadMat4(const Mat4& mat, const GLint& location) const;
	void uploadInt(int value, const GLint& location) const;

	void uploadVec2(const std::string& name, const Vec2& vec) const;
	void uploadVec3(const std::string& name, const Vec3& vec) const;
	void uploadVec4(const std::string& name, const Vec4& vec) const;
	void uploadMat4(const std::string& name, const Mat4& mat) const;
	void uploadInt(const std::string& name, int value) const;
	
	void uploadUniforms();

	void use() const;

private:
	GLuint program{};

	std::unordered_map<std::string, Vec2UniformEntry> vec2Uniforms;
	std::unordered_map<std::string, Vec3UniformEntry> vec3Uniforms;
	std::unordered_map<std::string, Vec4UniformEntry> vec4Uniforms;
	std::unordered_map<std::string, Mat4UniformEntry> mat4Uniforms;
	std::unordered_map<std::string, IntUniformEntry> intUniforms;

	std::vector<std::string> activeVec2Uniforms{};
	std::vector<std::string> activeVec3Uniforms{};
	std::vector<std::string> activeVec4Uniforms{};
	std::vector<std::string> activeMat4Uniforms{};
	std::vector<std::string> activeIntUniforms{};
};
