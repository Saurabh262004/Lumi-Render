#include <algorithm>

#include <lumi_render/Shader.hpp>

// Check if uniforms exist

bool Shader::isVec2Uniform(const std::string& id) const {
	return (vec2Uniforms.find(id) != vec2Uniforms.end());
}

bool Shader::isVec3Uniform(const std::string& id) const {
	return (vec3Uniforms.find(id) != vec3Uniforms.end());
}

bool Shader::isVec4Uniform(const std::string& id) const {
	return (vec4Uniforms.find(id) != vec4Uniforms.end());
}

bool Shader::isMat4Uniform(const std::string& id) const {
	return (mat4Uniforms.find(id) != mat4Uniforms.end());
}

bool Shader::isIntUniform(const std::string& id) const {
	return (intUniforms.find(id) != intUniforms.end());
}

// Check if uniforms are active

bool Shader::isVec2UniformActive(const std::string& id) const {
	return (std::find(activeVec2Uniforms.begin(), activeVec2Uniforms.end(), id) != activeVec2Uniforms.end());
}

bool Shader::isVec3UniformActive(const std::string& id) const {
	return (std::find(activeVec3Uniforms.begin(), activeVec3Uniforms.end(), id) != activeVec3Uniforms.end());
}

bool Shader::isVec4UniformActive(const std::string& id) const {
	return (std::find(activeVec4Uniforms.begin(), activeVec4Uniforms.end(), id) != activeVec4Uniforms.end());
}

bool Shader::isMat4UniformActive(const std::string& id) const {
	return (std::find(activeMat4Uniforms.begin(), activeMat4Uniforms.end(), id) != activeMat4Uniforms.end());
}

bool Shader::isIntUniformActive(const std::string& id) const {
	return (std::find(activeIntUniforms.begin(), activeIntUniforms.end(), id) != activeIntUniforms.end());
}

// Add uniforms to the map

void Shader::addVec2Uniform(const std::string& id, const Vec2& vec) {
	vec2Uniforms[id] = {
		vec,
		glGetUniformLocation(program, id.c_str())
	};
}

void Shader::addVec3Uniform(const std::string& id, const Vec3& vec) {
	vec3Uniforms[id] = {
		vec,
		glGetUniformLocation(program, id.c_str())
	};
}

void Shader::addVec4Uniform(const std::string& id, const Vec4& vec) {
	vec4Uniforms[id] = {
		vec,
		glGetUniformLocation(program, id.c_str())
	};
}

void Shader::addMat4Uniform(const std::string& id, const Mat4& mat) {
	mat4Uniforms[id] = {
		mat,
		glGetUniformLocation(program, id.c_str())
	};
}

void Shader::addIntUniform(const std::string& id, int value) {
	intUniforms[id] = {
		value,
		glGetUniformLocation(program, id.c_str())
	};
}

// Activate uniforms

void Shader::activateVec2Uniform(const std::string& id) {
	if (!isVec2Uniform(id) || isVec2UniformActive(id)) return;

	activeVec2Uniforms.push_back(id);
}

void Shader::activateVec3Uniform(const std::string& id) {
	if (!isVec3Uniform(id) || isVec3UniformActive(id)) return;

	activeVec3Uniforms.push_back(id);
}

void Shader::activateVec4Uniform(const std::string& id) {
	if (!isVec4Uniform(id) || isVec4UniformActive(id)) return;

	activeVec4Uniforms.push_back(id);
}

void Shader::activateMat4Uniform(const std::string& id) {
	if (!isMat4Uniform(id) || isMat4UniformActive(id)) return;

	activeMat4Uniforms.push_back(id);
}

void Shader::activateIntUniform(const std::string& id) {
	if (!isIntUniform(id) || isIntUniformActive(id)) return;

	activeIntUniforms.push_back(id);
}

// Deactivate uniforms

void Shader::deactivateVec2Uniform(const std::string& id) {
	if (!isVec2UniformActive(id)) return;

	activeVec2Uniforms.erase(
		std::remove(
			activeVec2Uniforms.begin(),
			activeVec2Uniforms.end(),
			id
		),
		activeVec2Uniforms.end()
	);
}

void Shader::deactivateVec3Uniform(const std::string& id) {
	if (!isVec3UniformActive(id)) return;

	activeVec3Uniforms.erase(
		std::remove(
			activeVec3Uniforms.begin(),
			activeVec3Uniforms.end(),
			id
		),
		activeVec3Uniforms.end()
	);
}

void Shader::deactivateVec4Uniform(const std::string& id) {
	if (!isVec4UniformActive(id)) return;

	activeVec4Uniforms.erase(
		std::remove(
			activeVec4Uniforms.begin(),
			activeVec4Uniforms.end(),
			id
		),
		activeVec4Uniforms.end()
	);
}

void Shader::deactivateMat4Uniform(const std::string& id) {
	if (!isMat4UniformActive(id)) return;

	activeMat4Uniforms.erase(
		std::remove(
			activeMat4Uniforms.begin(),
			activeMat4Uniforms.end(),
			id
		),
		activeMat4Uniforms.end()
	);
}

void Shader::deactivateIntUniform(const std::string& id) {
	if (!isIntUniformActive(id)) return;

	activeIntUniforms.erase(
		std::remove(
			activeIntUniforms.begin(),
			activeIntUniforms.end(),
			id
		),
		activeIntUniforms.end()
	);
}

// Upload uniforms by location

void Shader::uploadVec2(const Vec2& vec, const GLint& location) const {
	glUniform2f(location, vec.x, vec.y);
}

void Shader::uploadVec3(const Vec3& vec, const GLint& location) const {
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::uploadVec4(const Vec4& vec, const GLint& location) const {
	glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void Shader::uploadMat4(const Mat4& mat, const GLint& location) const {
	glUniformMatrix4fv(location, 1, GL_FALSE, mat.data());
}

void Shader::uploadInt(int value, const GLint& location) const {
	glUniform1i(location, value);
}

// Upload uniforms by name

void Shader::uploadVec2(const std::string& name, const Vec2& vec) const {
	GLint location = glGetUniformLocation(program, name.c_str());
	glUniform2f(location, vec.x, vec.y);
}

void Shader::uploadVec3(const std::string& name, const Vec3& vec) const {
	GLint location = glGetUniformLocation(program, name.c_str());
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::uploadVec4(const std::string& name, const Vec4& vec) const {
	GLint location = glGetUniformLocation(program, name.c_str());
	glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void Shader::uploadMat4(const std::string& name, const Mat4& mat) const {
	GLint location = glGetUniformLocation(program, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, mat.data());
}

void Shader::uploadInt(const std::string& name, int value) const {
	GLint location = glGetUniformLocation(program, name.c_str());
	glUniform1i(location, value);
}

// Upload all active uniforms

void Shader::uploadUniforms() {
	for (auto& id : activeVec2Uniforms) uploadVec2(vec2Uniforms.at(id).vec, vec2Uniforms.at(id).location);
	for (auto& id : activeVec3Uniforms) uploadVec3(vec3Uniforms.at(id).vec, vec3Uniforms.at(id).location);
	for (auto& id : activeVec4Uniforms) uploadVec4(vec4Uniforms.at(id).vec, vec4Uniforms.at(id).location);
	for (auto& id : activeMat4Uniforms) uploadMat4(mat4Uniforms.at(id).mat, mat4Uniforms.at(id).location);
	for (auto& id : activeIntUniforms) uploadInt(intUniforms.at(id).value, intUniforms.at(id).location);
}
