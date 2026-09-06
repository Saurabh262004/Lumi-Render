#pragma once

#include <cstddef>
#include <string>

#include <glad/glad.h>

class Texture {
public:
	explicit Texture(const std::string& path, bool flipVertically = true);

	static Texture fromMemory(const unsigned char* buffer, std::size_t size, bool flipVertically = true);

	static Texture fromRawBGRA(const unsigned char* data, int width, int height);

	~Texture();

	void bind(GLuint unit = 0) const;

	GLuint id() const { return textureID; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;

private:
	Texture() = default;

	void upload(const unsigned char* pixels, GLenum format, GLenum internalFormat);

	GLuint textureID{0};
	int width{0};
	int height{0};
	int channels{0};
};
