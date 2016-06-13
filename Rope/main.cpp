#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>
#include <cassert>
#include "Maths.h"
#include "Rope.h"
#include "Integrator.h"
#include "Camera.h"
#include "Picker.h"
#include "DebugRender.h"

int main(int argc, char** argv)
{
	// Make sure the OpenGL context has a depth buffer
	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 24;

	// Create our SFML window
	sf::RenderWindow window(sf::VideoMode(800, 600), "Rope Simulation", sf::Style::Default, contextSettings);
	window.setVerticalSyncEnabled(true);

	// Make it the active window for GL calls
	window.setActive();

	// Set up the GL state
	DebugRender::Initialise(window.getSize().x, window.getSize().y);

	// Build a rope with a fixed first particle
	Rope rope;
	Vec3 startPos(0.f, 3.f, 0.f);
	rope.AddParticle(startPos, 0.f);
	static const int numParticles = 20;
	static const float separation = 0.25f;
	for (int i = 1; i < numParticles; ++i)
	{
		rope.AddParticle(startPos + Vec3(static_cast<float>(i) * separation, 0.f, 0.f), 1.f);
	}

	// Stateful objects for timing, fixed time-step integration, camera and mouse picking
	sf::Clock clock;
	Integrator integrator;
	Picker picker;
	Camera camera;

	// Main loop:
	while (window.isOpen())
	{
		// Get time delta and clamp to something sane
		float dt = clock.restart().asSeconds();
		const float clamp = 0.2f;
		if (dt > clamp)
		{
			dt = clamp;
		}

		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close the window
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			// Adjust the viewport to match the window size
			if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}

		// Process physics
		integrator.Integrate(rope, dt);

		// Clear the graphics buffers
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// Update camera and picking
		camera.Update(dt);
		picker.Pick(window, camera.GetPos(), camera.GetLookAt(), rope);

		// Render the rope
		rope.Render();

		// Render a ground plane
		DebugRender::Quad(Vec3(-5.f, -5.f, -5.f), Vec3(-5.f, -5.f, 5.f), Vec3(5.f, -5.f, 5.f), Vec3(5.f, -5.f, -5.f), sf::Color(128, 128, 128));

		// Display to the window
		window.display();
	}

	DebugRender::Shutdown();

	return 0;
}