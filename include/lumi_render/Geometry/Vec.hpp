#pragma once

#include <cmath>

struct Vec2 {
	float x = 0.0f;
	float y = 0.0f;

	Vec2 operator+(const Vec2& other) const {
		return {
			x + other.x,
			y + other.y
		};
	}

	Vec2& operator+=(const Vec2& other) {
		x += other.x;
		y += other.y;

		return *this;
	}

	Vec2 operator-(const Vec2& other) const {
		return {
			x - other.x,
			y - other.y
		};
	}

	Vec2& operator-=(const Vec2& other) {
		x -= other.x;
		y -= other.y;

		return *this;
	}

	Vec2 operator-() const {
		return {-x, -y};
	}

	Vec2 operator*(const Vec2& other) const {
		return {
			x * other.x,
			y * other.y
		};
	}

	Vec2& operator*=(const Vec2& other) {
		x *= other.x;
		y *= other.y;

		return *this;
	}

	Vec2 operator*(const float other) const {
		return {
			x * other,
			y * other
		};
	}

	Vec2& operator*=(const float other) {
		x *= other;
		y *= other;

		return *this;
	}

	Vec2 operator/(const Vec2& other) const {
		return {
			x / other.x,
			y / other.y
		};
	}

	Vec2& operator/=(const Vec2& other) {
		x /= other.x;
		y /= other.y;

		return *this;
	}

	Vec2 operator/(const float other) const {
		return {
			x / other,
			y / other
		};
	}

	Vec2& operator/=(const float other) {
		x /= other;
		y /= other;

		return *this;
	}

	static double dot(const Vec2& v1, const Vec2& v2) {
		return (v1.x * v2.x) + (v1.y * v2.y);
	}

	// Returns a scalar representing the Z-axis magnitude
	static float cross(const Vec2& v1, const Vec2& v2) {
		return (v1.x * v2.y) - (v1.y * v2.x);
	}

	float length() const {
		return std::sqrt(x*x + y*y);
	}
};

struct Vec3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vec3 operator+(const Vec3& other) const {
		return {
			x + other.x,
			y + other.y,
			z + other.z
		};
	}

	Vec3& operator+=(const Vec3& other) {
		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}

	Vec3 operator-(const Vec3& other) const {
		return {
			x - other.x,
			y - other.y,
			z - other.z
		};
	}

	Vec3& operator-=(const Vec3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;

		return *this;
	}

	Vec3 operator-() const {
		return {-x, -y, -z};
	}

	Vec3 operator*(const Vec3& other) const {
		return {
			x * other.x,
			y * other.y,
			z * other.z
		};
	}

	Vec3& operator*=(const Vec3& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;

		return *this;
	}

	Vec3 operator*(const float other) const {
		return {
			x * other,
			y * other,
			z * other
		};
	}

	Vec3& operator*=(const float other) {
		x *= other;
		y *= other;
		z *= other;

		return *this;
	}

	Vec3 operator/(const Vec3& other) const {
		return {
			x / other.x,
			y / other.y,
			z / other.z
		};
	}

	Vec3& operator/=(const Vec3& other) {
		x /= other.x;
		y /= other.y;
		z /= other.z;

		return *this;
	}

	Vec3 operator/(const float other) const {
		return {
			x / other,
			y / other,
			z / other
		};
	}

	Vec3& operator/=(const float other) {
		x /= other;
		y /= other;
		z /= other;

		return *this;
	}

	static double dot(const Vec3& v1, const Vec3& v2) {
		return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	}

	static Vec3 cross(const Vec3& v1, const Vec3& v2) {
		return Vec3{
			(v1.y * v2.z) - (v1.z * v2.y),
			(v1.z * v2.x) - (v1.x * v2.z),
			(v1.x * v2.y) - (v1.y * v2.x)
		};
	}

	float length() const {
		return std::sqrt(x*x + y*y + z*z);
	}
};

struct Vec4 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	Vec4 operator+(const Vec4& other) const {
		return {
			x + other.x,
			y + other.y,
			z + other.z,
			w + other.w
		};
	}

	Vec4& operator+=(const Vec4& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;

		return *this;
	}

	Vec4 operator-(const Vec4& other) const {
		return {
			x - other.x,
			y - other.y,
			z - other.z,
			w - other.w
		};
	}

	Vec4& operator-=(const Vec4& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;

		return *this;
	}

	Vec4 operator-() const {
		return {-x, -y, -z, -w};
	}

	Vec4 operator*(const Vec4& other) const {
		return {
			x * other.x,
			y * other.y,
			z * other.z,
			w * other.w
		};
	}

	Vec4& operator*=(const Vec4& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;

		return *this;
	}

	Vec4 operator*(const float other) const {
		return {
			x * other,
			y * other,
			z * other,
			w * other
		};
	}

	Vec4& operator*=(const float other) {
		x *= other;
		y *= other;
		z *= other;
		w *= other;

		return *this;
	}

	Vec4 operator/(const Vec4& other) const {
		return {
			x / other.x,
			y / other.y,
			z / other.z,
			w / other.w
		};
	}

	Vec4& operator/=(const Vec4& other) {
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;

		return *this;
	}

	Vec4 operator/(const float other) const {
		return {
			x / other,
			y / other,
			z / other,
			w / other
		};
	}

	Vec4& operator/=(const float other) {
		x /= other;
		y /= other;
		z /= other;
		w /= other;

		return *this;
	}

	static double dot(const Vec4& v1, const Vec4& v2) {
		return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
	}

	static Vec4 cross(const Vec4& v1, const Vec4& v2) {
		return Vec4{
			(v1.y * v2.z) - (v1.z * v2.y),
			(v1.z * v2.x) - (v1.x * v2.z),
			(v1.x * v2.y) - (v1.y * v2.x),
			0.0f
		};
	}

	float length() const {
		return std::sqrt(x*x + y*y + z*z + w*w);
	}
};
