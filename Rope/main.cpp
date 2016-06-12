
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>

#include <cassert>

#include "Maths.h"
#include "Rope.h"
#include "Integrator.h"
#include "DebugRender.h"


int main(int argc, char** argv)
{
	// Request a 24-bits depth buffer when creating the window
	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 24;

	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "Rope Simulation", sf::Style::Default, contextSettings);
	window.setVerticalSyncEnabled(true);

	// Make it the active window for OpenGL calls
	window.setActive();

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	// Enable back-face culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	// Set up lighting
	GLfloat	globalAmbient[] = { 0.3f, 0.3f, 0.3f, 1.f };
	GLfloat light0AmbDiff[] = { 0.9f, 0.9f, 0.7f, 1.f };
	GLfloat light0Pos[] = { 0.f, 0.f, 5.f, 1.0 };

	glEnable(GL_LIGHTING);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, light0AmbDiff);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glEnable(GL_LIGHT0);

	// Configure the viewport (the same size as the window)
	glViewport(0, 0, window.getSize().x, window.getSize().y);

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);
	gluPerspective(75.f, ratio, 1.f, 50.f);

	// Quadrics object for drawing shapes
	GLUquadric* quadric = gluNewQuadric();
	if (!quadric)
		return 1;

	Rope rope;
	Vec3 startPos(0.f, 3.f, 0.f);
	rope.AddParticle(startPos, 0.f);
	static const int numParticles = 10;
	static const float separation = 0.5f;
	for (int i = 1; i < numParticles; ++i)
	{
		rope.AddParticle(startPos + Vec3(static_cast<float>(i) * separation, 0.f, 0.f), 1.f);
	}
	rope.AddParticle(startPos + Vec3(static_cast<float>(numParticles) * separation, 0.f, 0.f), 20.f);

	Integrator integrator;

	// Create a clock for measuring the time elapsed
	sf::Clock clock;

	// Start main loop
	while (window.isOpen())
	{
		float dt = clock.restart().asSeconds();

		// Clamp dt in case we've been debugging
		const float clamp = 0.2f;
		if (dt > clamp)
		{
			dt = clamp;
		}

		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window : exit
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			// Adjust the viewport when the window is resized
			if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}

		// Clear the depth buffer
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// Set the camera matrix.
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPushMatrix();

		// Slowly rotate the camera
		static const float camRadius = 5.f;
		static float camTheta = PI / 2.f;
		camTheta += 0.25f * dt;
		while (camTheta > 2.f * PI)
		{
			camTheta -= 2.f * PI;
		}

		Vec3 camPos(camRadius * cosf(camTheta), 0.f, camRadius * sinf(camTheta));
		Vec3 camLookAt(0.f, 0.f, 0.f);

		gluLookAt(
			camPos.x, camPos.y, camPos.z,
			camLookAt.x, camLookAt.y, camLookAt.z,
			0.f, 1.f, 0.f);

		// Project the mouse position onto the plane
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);

		// Clamp the mouse position to the window, so we don't get crazy results from gluUnProject
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

		// Cast the mouse onto a plane around the last particle of the rope, with a normal of the camera forward vector (i.e. grab at a fixed distance)
		float t = (rope.m_particles.back().m_pos - mousePosFrontClipPlane).Dot(lookDir) / rayDir.Dot(lookDir);
		Vec3 castPoint = mousePosFrontClipPlane + rayDir * t;
		DebugRender::Sphere(*quadric, castPoint, 0.1f, sf::Color::Red);

		// Add a fake particle to the rope based on the mouse position
		bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		static Particle* mouseParticle = NULL;
		if (isMousePressed)
		{
			if (mouseParticle)
			{
				mouseParticle->m_pos = castPoint;
			}
			else
			{
				rope.AddParticle(castPoint, 0.f);
				mouseParticle = &rope.m_particles.back();
			}
		}
		else if (!isMousePressed && mouseParticle)
		{
			mouseParticle = NULL;
			rope.m_particles.pop_back();
		}

		integrator.Integrate(rope, dt);

		glMatrixMode(GL_MODELVIEW);

		rope.Render(*quadric);

		// Render a ground plane
		DebugRender::Quad(Vec3(-5.f, -5.f, -5.f), Vec3(-5.f, -5.f, 5.f), Vec3(5.f, -5.f, 5.f), Vec3(5.f, -5.f, -5.f), sf::Color(128, 128, 128));

		// Finally, display the rendered frame on screen
		window.display();
	}

	gluDeleteQuadric(quadric);

	return 0;
}