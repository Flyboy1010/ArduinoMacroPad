#pragma once

#include <glm/glm.hpp>

/* ORTHOGRAPHIC CAMERA */

class OrthographicCamera
{
public:
	OrthographicCamera();
	OrthographicCamera(const glm::vec2& position);

	const glm::vec2& GetPosition() const { return m_position; }
	const glm::mat4& GetProjection() const { return m_projection; }
	glm::mat4 GetTransform() const;

	void SetPosition(const glm::vec2& position) { m_position = position; }
	void SetProjection(float left, float right, float bottom, float top);

private:
	glm::vec2 m_position;
	glm::mat4 m_projection;

	float m_left, m_right;
	float m_top, m_bottom;
};

/* CAMERA */

class Camera
{
public:
	Camera();
	Camera(const glm::vec3& position);

	const glm::vec3& GetPosition() const { return m_position; }
	const glm::mat4& GetProjection() const { return m_projection; }
	const glm::mat4& GetTransform() const { return m_transform; }
	const glm::vec3& GetUp() const { return m_up; }
	const glm::vec3& GetRight() const { return m_right; }
	const glm::vec3& GetForward() const { return m_forward; }
	float GetPitch() const { return m_pitch; }
	float GetYaw() const { return m_yaw; }

	void SetPosition(const glm::vec3& position) { m_position = position; }
	void SetPerspectiveProjection(float fov, float aspectRatio, float near, float far);
	void SetOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
	void SetWorldUp(const glm::vec3& worldUp) { m_worldUp = worldUp; }

	void LookAt(const glm::vec3& point);

protected:
	glm::vec3 m_position;
	glm::mat4 m_projection;
	glm::mat4 m_transform;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_forward;
	glm::vec3 m_worldUp = { 0.0f, 1.0f, 0.0f };

	float m_pitch, m_yaw;

	float m_fov;
	float m_aspectRatio;
	float m_near, m_far;
};

/* FPS CAMERA */

class FpsCamera : public Camera
{
public:
	FpsCamera();
	FpsCamera(const glm::vec3& position);

	float GetSpeed() const { return m_speed; }
	void SetSpeed(float speed) { m_speed = speed; }

	void Update(float delta);

private:
	float m_speed;
	glm::vec2 m_lastMousePosition;
};

/* EDITOR CAMERA */

class EditorCamera : public Camera
{
public:
	EditorCamera();
	EditorCamera(const glm::vec3& position);

	void Update(float delta);

private:
	glm::vec3 m_focalPoint;
	float m_distanceToFocalPoint;

	glm::vec2 m_lastMousePosition;
};