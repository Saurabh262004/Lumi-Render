#include <stdexcept>
#include <utility>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <lumi_render/Texture.hpp>

Texture::Texture(const std::string& path, bool flipVertically) {
	stbi_set_flip_vertically_on_load(flipVertically);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data) {
		throw std::runtime_error("Failed to load texture: " + path + " (" + stbi_failure_reason() + ")");
	}

	GLenum format = GL_RGB;
	if (channels == 1)      format = GL_RED;
	else if (channels == 3) format = GL_RGB;
	else if (channels == 4) format = GL_RGBA;

	upload(data, format, format);
	stbi_image_free(data);
}

Texture Texture::fromMemory(const unsigned char* buffer, std::size_t size, bool flipVertically) {
	stbi_set_flip_vertically_on_load(flipVertically);

	Texture tex;
	unsigned char* data = stbi_load_from_memory(buffer, static_cast<int>(size), &tex.width, &tex.height, &tex.channels, 0);
	if (!data) {
		throw std::runtime_error(std::string("Failed to load embedded texture from memory (") + stbi_failure_reason() + ")");
	}

	GLenum format = GL_RGB;
	if (tex.channels == 1)      format = GL_RED;
	else if (tex.channels == 3) format = GL_RGB;
	else if (tex.channels == 4) format = GL_RGBA;

	tex.upload(data, format, format);
	stbi_image_free(data);

	return tex;
}

Texture Texture::fromRawBGRA(const unsigned char* data, int width, int height) {
	Texture tex;
	tex.width = width;
	tex.height = height;
	tex.channels = 4;

	tex.upload(data, GL_BGRA, GL_RGBA);

	return tex;
}

void Texture::upload(const unsigned char* pixels, GLenum format, GLenum internalFormat) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
	if (textureID != 0) glDeleteTextures(1, &textureID);
}

void Texture::bind(GLuint unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

Texture::Texture(Texture&& other) noexcept :
	textureID{std::exchange(other.textureID, 0)},
	width{other.width}, height{other.height}, channels{other.channels}
{}

Texture& Texture::operator=(Texture&& other) noexcept {
	if (this != &other) {
		if (textureID != 0) glDeleteTextures(1, &textureID);

		textureID = std::exchange(other.textureID, 0);
		width = other.width;
		height = other.height;
		channels = other.channels;
	}

	return *this;
}
