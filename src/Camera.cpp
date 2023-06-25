#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include "Core/Input.h"

/* ORTHOGRAPHIC CAMERA */

OrthographicCamera::OrthographicCamera()
{
	m_position = { 0.0f, 0.0f };
	m_projection = glm::mat4(1.0f);
	m_left = 0.0f, m_right = 0.0f;
	m_top = 0.0f, m_bottom = 0.0f;
}

OrthographicCamera::OrthographicCamera(const glm::vec2& position)
{
	m_position = position;
	m_projection = glm::mat4(1.0f);
	m_left = 0.0f, m_right = 0.0f;
	m_top = 0.0f, m_bottom = 0.0f;
}

glm::mat4 OrthographicCamera::GetTransform() const
{
	return glm::translate(glm::mat4(1.0f), -glm::vec3(m_position, 0.0f));
}

void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
{
	m_left = left; m_right = right;
	m_bottom = bottom; m_top = top;
	m_projection = glm::ortho(left, right, bottom, top);
}

/* CAMERA */

Camera::Camera()
{
	m_yaw = 45.0f, m_pitch = 45.0f;
	m_position = { 0.0f, 0.0f, 0.0f };
	m_up = { 0.0f, 1.0f, 0.0f };
	m_forward = { 0.0f, 0.0f, -1.0f };
	m_right = { 1.0f, 0.0f, 0.0f };
	SetPerspectiveProjection(70.0f, 16.0f / 9.0f, 0.01f, 1000.0f);
}

Camera::Camera(const glm::vec3& position)
{
	m_yaw = 45.0f, m_pitch = 45.0f;
	m_position = position;
	m_up = { 0.0f, 1.0f, 0.0f };
	m_forward = { 0.0f, 0.0f, -1.0f };
	m_right = { 1.0f, 0.0f, 0.0f };
	SetPerspectiveProjection(70.0f, 16.0f / 9.0f, 0.01f, 1000.0f);
}

void Camera::SetPerspectiveProjection(float fov, float aspectRatio, float near, float far)
{
	m_fov = fov;
	m_aspectRatio = aspectRatio;
	m_near = near;
	m_far = far;
	m_projection = glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::SetOrthographicProjection(float left, float right, float bottom, float top, float near, float far)
{
	m_projection = glm::ortho(left, right, bottom, top, near, far);
}

void Camera::LookAt(const glm::vec3& point)
{
	// calculate vectors

	m_forward = glm::normalize(m_position - point);
	m_right = glm::normalize(glm::cross(m_worldUp, m_forward));
	m_up = glm::normalize(glm::cross(m_forward, m_right));

	// calculate transform

	m_transform = glm::lookAt(m_position, (m_position - m_forward), m_up);
}

/* FPS CAMERA */

FpsCamera::FpsCamera() : Camera()
{
	m_speed = 5.0f;
	m_lastMousePosition = { 0.0f, 0.0f };
}

FpsCamera::FpsCamera(const glm::vec3& position) : Camera(position)
{
	m_speed = 5.0f;
	m_lastMousePosition = { 0.0f, 0.0f };
}

void FpsCamera::Update(float delta)
{
	// get input singleton

	const Input& input = Input::Get();

	// calculate how much the mouse has moved since last time

	const glm::vec2& mousePosition = input.GetMousePosition();
	glm::vec2 mousePositionDelta = mousePosition - m_lastMousePosition;
	m_lastMousePosition = mousePosition;

	// tranlate it into camera yaw and pitch

	const float sensitivity = 0.175f;

	m_yaw += sensitivity * mousePositionDelta.x;
	m_pitch -= sensitivity * mousePositionDelta.y;

	// clamp pitch

	if (m_pitch < 1.0f) m_pitch = 1.0f;
	else if (m_pitch > 179.0f) m_pitch = 179.0f;

	/* CALCULATE FORWARD UP AND RIGHT VECTORS */

	// get the forward direction (as unit vector)

	m_forward.x = glm::sin(glm::radians(m_pitch)) * glm::cos(glm::radians(m_yaw));
	m_forward.y = glm::cos(glm::radians(m_pitch));
	m_forward.z = glm::sin(glm::radians(m_pitch)) * glm::sin(glm::radians(m_yaw));
	m_forward = glm::normalize(m_forward);

	// get the right direction (as unit vector)

	m_right = glm::normalize(glm::cross(m_worldUp, m_forward));

	// get the up direction (as unit vector)

	m_up = glm::normalize(glm::cross(m_forward, m_right));

	// calculate transform

	m_transform = glm::lookAt(m_position, (m_position - m_forward), m_up);

	/* HANDLE INPUT */

	if (input.IsKeyPressed(Key::D))
		m_position += m_speed * delta * m_right;
	else if (input.IsKeyPressed(Key::A))
		m_position -= m_speed * delta * m_right;

	if (input.IsKeyPressed(Key::W))
		m_position -= m_speed * delta * m_forward;
	else if (input.IsKeyPressed(Key::S))
		m_position += m_speed * delta * m_forward;

	if (input.IsKeyPressed(Key::E))
		m_position += m_speed * delta * m_worldUp;
	else if (input.IsKeyPressed(Key::Q))
		m_position -= m_speed * delta * m_worldUp;
}

/* EDITOR CAMERA */

EditorCamera::EditorCamera() : Camera()
{
	m_yaw = 45.0f, m_pitch = 45.0f;
	m_focalPoint = { 0.0f, 0.0f, 0.0f };
	m_lastMousePosition = { 0.0f, 0.0f };
	m_distanceToFocalPoint = 5.0f;
}

EditorCamera::EditorCamera(const glm::vec3& position) : Camera(position)
{
	m_yaw = 45.0f, m_pitch = 45.0f;
	m_focalPoint = { 0.0f, 0.0f, 0.0f };
	m_lastMousePosition = { 0.0f, 0.0f };
	m_distanceToFocalPoint = 5.0f;
}

void EditorCamera::Update(float delta)
{
	// get input singleton

	const Input& input = Input::Get();

	// check if the left alt is pressed so you can move the camera

	if (input.IsKeyPressed(Key::LeftAlt))
	{
		const glm::vec2& mousePosition = input.GetMousePosition();
		glm::vec2 mousePositionDelta = mousePosition - m_lastMousePosition;
		m_lastMousePosition = mousePosition;
		float scroll = input.GetMouseScrollDelta();

		if (input.IsMousePressed(MouseButton::Left)) // rotate the camera around the focal point
		{
			const float sensitivity = 0.175f;

			m_yaw += mousePositionDelta.x * sensitivity;
			m_pitch -= mousePositionDelta.y * sensitivity;

			// clamp pitch

			if (m_pitch < 1.0f) m_pitch = 1.0f;
			else if (m_pitch > 179.0f) m_pitch = 179.0f;
		}
		else if (input.IsMousePressed(MouseButton::Right)) // pan the focal point
		{
			const float sensitivity = 0.01f;

			m_focalPoint += (m_right * mousePositionDelta.x - m_up * mousePositionDelta.y) * sensitivity;
		}
		else if (scroll != 0.0f)
		{
			const float sensitivity = 1.0f;

			m_distanceToFocalPoint -= scroll * sensitivity;
		
			if (m_distanceToFocalPoint < 0.1f)
				m_distanceToFocalPoint = 0.1f;
		}
	}

	/* CALCULATE CAMERA POSITION AND VECTORS */

	// get the forward direction (as unit vector)

	m_forward.x = glm::sin(glm::radians(m_pitch)) * glm::cos(glm::radians(m_yaw));
	m_forward.y = glm::cos(glm::radians(m_pitch));
	m_forward.z = glm::sin(glm::radians(m_pitch)) * glm::sin(glm::radians(m_yaw));
	m_forward = glm::normalize(m_forward);

	m_position = (m_forward * m_distanceToFocalPoint) + m_focalPoint;

	// get the right direction (as unit vector)

	m_right = glm::normalize(glm::cross(m_worldUp, m_forward));

	// get the up direction (as unit vector)

	m_up = glm::normalize(glm::cross(m_forward, m_right));

	// calculate transform

	m_transform = glm::lookAt(m_position, (m_position - m_forward), m_up);
}
