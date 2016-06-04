
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>

struct Vec3
{
	float x, y, z;

	Vec3(float x, float y, float z)
		: x(x)
		, y(y)
		, z(z)
	{
	}

	Vec3 operator+(const Vec3& rhs) const
	{
		return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	Vec3 operator-(const Vec3& rhs) const
	{
		return *this + (-rhs);
	}

	Vec3 operator*(float s) const
	{
		return Vec3(s * x, s * y, s * z);
	}

	Vec3 operator/(float s) const
	{
		float invS = 1.f / s;
		return *this * invS;
	}

	Vec3& operator+=(const Vec3& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	Vec3& operator-=(const Vec3& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	Vec3& operator*=(float s)
	{
		*this = *this * s;
		return *this;
	}

	Vec3& operator/=(float s)
	{
		*this = *this / s;
		return *this;
	}

	float Dot(const Vec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	float MagSqr() const
	{
		return Dot(*this);
	}

	float Mag() const
	{
		return sqrtf(MagSqr());
	}

	Vec3 GetNormalised() const
	{
		return *this / Mag();
	}
};

class Particle
{
public:
	Particle(Vec3 pos, Particle* prev)
		: m_pos(pos)
		, m_vel(0.f, 0.f, 0.f)
		, m_dist(prev ? (pos - prev->m_pos).Mag() : 0.f)
	{
	}

	Vec3 m_pos;
	Vec3 m_vel;
	float m_dist;
};

#include <vector>

class Rope
{
public:
	void Integrate(float dt)
	{
		for (Particle& p : m_particles)
		{
			static const Vec3 gravity(0.f, -9.8f, 0.f);
			p.m_vel += gravity * dt;
			p.m_pos += p.m_vel * dt;
		}

		if (!m_particles.empty())
		{
			m_particles.front().m_pos = Vec3(0.f, 3.f, 0.f);
			m_particles.front().m_vel = Vec3(0.f, 0.f, 0.f);
		}
	}

	void SolveConstraints(float dt)
	{
		static const int numIterations = 10;
		for (int iter = 0; iter < numIterations; ++iter)
		{
			for (int partIndex = 1; partIndex < m_particles.size(); ++partIndex)
			{
				Particle& particle = m_particles[partIndex];
				Particle& prevParticle = m_particles[partIndex - 1];
				Vec3 delta = particle.m_pos - prevParticle.m_pos;
				float myDist = delta.Mag();
				if (myDist > particle.m_dist)
				{
					Vec3 n = delta.GetNormalised();
					particle.m_pos = prevParticle.m_pos + n * particle.m_dist;
				}
			}
		}
	}

	void Render() const
	{
		if (!m_particles.empty())
		{
			glLoadIdentity();
			glBegin(GL_LINE_STRIP);
			//glBegin(GL_TRIANGLES);
			for (const Particle& p : m_particles)
			{
				glVertex3fv((const GLfloat*)&p.m_pos);
			}
			glEnd();
		}
	}

	void AddParticle(const Vec3& pos)
	{
		Particle* prev = m_particles.empty() ? NULL : &m_particles.back();
		m_particles.push_back(Particle(pos, prev));
	}

	std::vector<Particle> m_particles;
};

class Integrator
{
public:
	Integrator()
		: m_timeAccumulator(0.f)
	{
	}

	void Integrate(Rope& rope, float dt)
	{
		static const float timeStep = 1.f / 60.f;
		m_timeAccumulator += dt;
		while (m_timeAccumulator >= timeStep)
		{
			m_timeAccumulator -= timeStep;
			rope.Integrate(timeStep);
		}
	}

private:
	float m_timeAccumulator;
};

int main(int argc, char** argv)
{
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "Rope Simulation");
	window.setVerticalSyncEnabled(true);

	// Make it the active window for OpenGL calls
	window.setActive();

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	// Disable lighting
	glDisable(GL_LIGHTING);

	// Configure the viewport (the same size as the window)
	glViewport(0, 0, window.getSize().x, window.getSize().y);

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);
	glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
	// Enable position vertex components
	//   glEnableClientState(GL_VERTEX_ARRAY);

	glPushMatrix();

	// Disable normal and color vertex components
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);



	Rope rope;
	rope.AddParticle(Vec3(0.f, 3.f, 0.f));
	rope.AddParticle(Vec3(1.f, 3.f, 0.f));
	rope.AddParticle(Vec3(2.f, 3.f, 0.f));
	rope.AddParticle(Vec3(3.f, 3.f, 0.f));

	Integrator integrator;

	// Create a clock for measuring the time elapsed
	sf::Clock clock;

	// Start main loop
	while (window.isOpen())
	{
		float dt = clock.restart().asSeconds();

		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window : exit
			if (event.type == sf::Event::Closed)
				window.close();

			// Adjust the viewport when the window is resized
			if (event.type == sf::Event::Resized)
				glViewport(0, 0, event.size.width, event.size.height);
		}

		// Clear the depth buffer
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// Set the camera matrix.
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPushMatrix();
		Vec3 vCamPos = { 0.f, 0.f, -5.f };
		Vec3 vCamLook = { 0.f, 0.5f, 0.f };

		gluLookAt(
			vCamPos.x, vCamPos.y, vCamPos.z,
			vCamLook.x, vCamLook.y, vCamLook.z,
			0.f, 1.f, 0.f);

		glMatrixMode(GL_MODELVIEW);

		unsigned int rgba = 0xff0000ff;
		glColor4ubv((const GLubyte*)&rgba);

		integrator.Integrate(rope, dt);
		rope.SolveConstraints(dt);
		rope.Render();

		// Finally, display the rendered frame on screen
		window.display();
	}

	return 0;
}