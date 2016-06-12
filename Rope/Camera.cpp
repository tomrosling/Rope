#include "Camera.h"
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Mouse.hpp>
#include <gl/GLU.h>
#include "Maths.h"

Camera::Camera()
	: m_angle(PI / 2.f)
	, m_pos(0.f, 0.f, 0.f)
	, m_lookAt(0.f, 0.f, 0.f)
{
}

void Camera::Update(float dt)
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPushMatrix();

	// Slowly rotate the camera, as long as we're not picking
	static const float radius = 5.f;
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		m_angle += 0.25f * dt;
		while (m_angle > 2.f * PI)
		{
			m_angle -= 2.f * PI;
		}
	}

	m_pos = Vec3(radius * cosf(m_angle), 0.f, radius * sinf(m_angle));
	m_lookAt = Vec3(0.f, 0.f, 0.f);

	gluLookAt(
		m_pos.x, m_pos.y, m_pos.z,
		m_lookAt.x, m_lookAt.y, m_lookAt.z,
		0.f, 1.f, 0.f);
}
