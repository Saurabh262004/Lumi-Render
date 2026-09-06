#pragma once

#include <lumi/Geometry/Vec.hpp>
#include <lumi/Geometry/Mat.hpp>

class Camera {
private:
	Vec3 position;

	float pitch{0.0f}, yaw{0.0f}, roll{0.0f};

	float fov = 45.0f * (M_PI / 180.0f);

	float nearPlane{0.001f};
	float farPlane{1000.0f};

	float viewportWidth{800.0f};
	float viewportHeight{600.0f};

	bool perspective{true};

	Mat4 viewProjection{};

public:
	void setPosition(Vec3 position) {
		this->position = position;
	}

	void setPitch(float pitch) {
		constexpr float limit = 89.0f * static_cast<float>(M_PI) / 180.0f;
		this->pitch = (pitch > limit) ? limit : (pitch < -limit ? -limit : pitch);
	}

	void setYaw(float yaw) {
		this->yaw = yaw;
	}

	void setRoll(float roll) {
		this->roll = roll;
	}

	void setFOV(float degrees) {
		fov = degrees * (M_PI / 180.0f);
	}

	void setNearPlane(float nearPlane) {
		this->nearPlane = nearPlane;
	}

	void setFarPlane(float farPlane) {
		this->farPlane = farPlane;
	}

	void setViewportResolution(float width, float height) {
		viewportWidth = width;
		viewportHeight = height;
	}

	void setUsePerspective(bool value) {
		perspective = value;
	}

	Vec3 getPosition() const {
		return position;
	}

	float getPitch() const {
		return pitch;
	}

	float getYaw() const {
		return yaw;
	}

	float getRoll() const {
		return roll;
	}

	Mat4 getOrientation() const {
		return Mat4::rotateY(yaw) * Mat4::rotateX(pitch) * Mat4::rotateZ(roll);
	}

	Vec3 getForward() const {
		Vec4 f = getOrientation() * Vec4{0.0f, 0.0f, -1.0f, 0.0f};
		return Vec3{f.x, f.y, f.z};
	}

	Vec3 getRight() const {
		Vec4 r = getOrientation() * Vec4{1.0f, 0.0f, 0.0f, 0.0f};
		return Vec3{r.x, r.y, r.z};
	}

	Vec3 getUp() const {
		Vec4 u = getOrientation() * Vec4{0.0f, 1.0f, 0.0f, 0.0f};
		return Vec3{u.x, u.y, u.z};
	}

	float getFOV() const {
		return fov * (180 / M_PI);
	}

	float getNearPlane() const {
		return nearPlane;
	}

	float getFarPlane() const {
		return farPlane;
	}

	Mat4 getViewProjection() const {
		return viewProjection;
	}

	bool getUsePerspective() const {
		return perspective;
	}

	void updateViewProjection() {
		Mat4 rotation = Mat4::rotateZ(-roll) * Mat4::rotateX(-pitch) * Mat4::rotateY(-yaw);
		Mat4 translation = Mat4::translate(-position);
		Mat4 view = rotation * translation;

		float aspect = viewportWidth / viewportHeight;

		Mat4 projection;    
		if (perspective) {
			projection = Mat4::perspective(fov, aspect, nearPlane, farPlane);
		} else {
			float halfWidth = viewportWidth * 0.5f;
			float halfHeight = viewportHeight * 0.5f;

			projection = Mat4::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
		}

		viewProjection = projection * view;
	}
};
