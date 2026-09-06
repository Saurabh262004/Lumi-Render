#pragma once

#include <lumi/Geometry/Vec.hpp>
#include <cmath>

struct Mat4 {
	float m[16]{}; // zero-initialized, column-major: data[col*4 + row]

	static Mat4 identity() {
		Mat4 result;

		result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1.0f;

		return result;
	}

	Mat4 operator*(const Mat4& o) const {
		Mat4 result;

		for (int c = 0; c < 4; ++c) {
			for (int row = 0; row < 4; ++row) {
				float sum = 0.0f;

				for (int k = 0; k < 4; ++k) {
					sum += m[k * 4 + row] * o.m[c * 4 + k];
				}

				result.m[c * 4 + row] = sum;
			}
		}

		return result;
	}

	Vec4 operator*(const Vec4& v) const {
		return Vec4{
			m[0]*v.x + m[4]*v.y + m[8]*v.z  + m[12]*v.w,
			m[1]*v.x + m[5]*v.y + m[9]*v.z  + m[13]*v.w,
			m[2]*v.x + m[6]*v.y + m[10]*v.z + m[14]*v.w,
			m[3]*v.x + m[7]*v.y + m[11]*v.z + m[15]*v.w
		};
	}

	static Mat4 translate(const Vec3& t) {
		Mat4 result = identity();

		result.m[12] = t.x; result.m[13] = t.y; result.m[14] = t.z;

		return result;
	}

	static Mat4 scale(const Vec3& s) {
		Mat4 result = identity();

		result.m[0] = s.x; result.m[5] = s.y; result.m[10] = s.z;

		return result;
	}

	static Mat4 rotateX(float radians) {
		Mat4 result = identity();

		float cos = std::cos(radians);
		float sin = std::sin(radians);

		result.m[5] = cos; result.m[6] = sin;
		result.m[9] = -sin; result.m[10] = cos;

		return result;
	}

	static Mat4 rotateY(float radians) {
		Mat4 result = identity();

		float cos = std::cos(radians);
		float sin = std::sin(radians);

		result.m[0] = cos; result.m[2] = -sin;
		result.m[8] = sin; result.m[10] = cos;

		return result;
	}

	static Mat4 rotateZ(float radians) {
		Mat4 result = identity();

		float cos = std::cos(radians);
		float sin = std::sin(radians);

		result.m[0] = cos; result.m[1] = sin;
		result.m[4] = -sin; result.m[5] = cos;

		return result;
	}

	// (Rodrigues' rotation formula)
	static Mat4 rotate(float radians, Vec3 axis) {
		float len = std::sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);

		axis.x /= len; axis.y /= len; axis.z /= len;

		float c = std::cos(radians), s = std::sin(radians), omc = 1.0f - c;

		Mat4 result = identity();

		result.m[0] = c + axis.x*axis.x*omc;
		result.m[1] = axis.y*axis.x*omc + axis.z*s;
		result.m[2] = axis.z*axis.x*omc - axis.y*s;

		result.m[4] = axis.x*axis.y*omc - axis.z*s;
		result.m[5] = c + axis.y*axis.y*omc;
		result.m[6] = axis.z*axis.y*omc + axis.x*s;

		result.m[8]  = axis.x*axis.z*omc + axis.y*s;
		result.m[9]  = axis.y*axis.z*omc - axis.x*s;
		result.m[10] = c + axis.z*axis.z*omc;

		return result;
	}

	static Mat4 perspective(float fovRadians, float aspect, float near, float far) {
		Mat4 result{};

		float f = 1.0f / std::tan(fovRadians / 2.0f);

		result.m[0]  = f / aspect;							// col 0, row 0
		result.m[5]  = f;									// col 1, row 1
		result.m[10] = (far + near) / (near - far);			// col 2, row 2
		result.m[11] = -1.0f;								// col 2, row 3
		result.m[14] = (2 * far * near) / (near - far);		// col 3, row 2

		return result;
	}

	static Mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
		Mat4 result{};

		result.m[0]  =  2.0f / (right - left);                  // col 0, row 0
		result.m[5]  =  2.0f / (top - bottom);                  // col 1, row 1
		result.m[10] = -2.0f / (far - near);                    // col 2, row 2
		result.m[12] = -(right + left) / (right - left);        // col 3, row 0
		result.m[13] = -(top + bottom) / (top - bottom);        // col 3, row 1
		result.m[14] = -(far + near) / (far - near);            // col 3, row 2
		result.m[15] =  1.0f;                                   // col 3, row 3

		return result;
	}

	const float* data() const {
		return &m[0];
	}
};
