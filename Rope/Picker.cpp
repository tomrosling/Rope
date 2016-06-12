#include "Picker.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>
#include "Maths.h"
#include "Rope.h"

Picker::Picker()
	: m_planeW(FLT_MAX)
{
}

void Picker::Pick(const sf::Window& window, const Vec3& camPos, const Vec3& camLookAt, Rope& rope)
{
	Particle* lastParticle = rope.GetLastParticle();
	if (!lastParticle)
		return;

	// Get the mouse position and clamp it to the window, so we don't get crazy results from gluUnProject
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2u windowSize = window.getSize();
	mousePos.x = Clamp(mousePos.x, 0, static_cast<int>(windowSize.x));
	mousePos.y = Clamp(mousePos.y, 0, static_cast<int>(windowSize.y));

	Vec3 mousePos3(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), 0.f);

	// Get the mouse position in world space (at the front of the view frustum)
	double modelMtx[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMtx);

	double projMtx[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projMtx);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	double castX, castY, castZ;
	gluUnProject(static_cast<float>(mousePos.x), static_cast<float>(windowSize.y - mousePos.y), 0.f,
		modelMtx, projMtx, viewport,
		&castX, &castY, &castZ);

	Vec3 mousePosFrontClipPlane(static_cast<float>(castX), static_cast<float>(castY), static_cast<float>(castZ));
	Vec3 rayDir = (mousePosFrontClipPlane - camPos).GetNormalised();
	Vec3 lookDir = (camLookAt - camPos).GetNormalised();

	float ropePlaneW = lastParticle->m_pos.Dot(lookDir);
	float usePlaneW = (m_planeW == FLT_MAX) ? ropePlaneW : m_planeW;

	// Cast the mouse onto a plane around the last particle of the rope, with a normal of the camera forward vector (i.e. grab at a fixed distance)
	float t = (usePlaneW - mousePosFrontClipPlane.Dot(lookDir)) / rayDir.Dot(lookDir);
	Vec3 castPoint = mousePosFrontClipPlane + rayDir * t;

	// Drag the last particle to the mouse position
	bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	if (isMousePressed)
	{
		lastParticle->m_pos = castPoint;

		// If we just pressed the mouse down, store the depth value
		if (m_planeW == FLT_MAX)
		{
			m_planeW = ropePlaneW;
		}
	}
	else
	{
		// Reset when releasing the mouse
		m_planeW = FLT_MAX;
	}
}