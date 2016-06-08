
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>

#include <cassert>

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
		float invS = (s > FLT_EPSILON) ? 1.f / s : 0.f;
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

	float operator[](int i) const
	{
		return *(&x + i);
	}

	float& operator[](int i)
	{
		return *(&x + i);
	}

	float Dot(const Vec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	Vec3 Cross(const Vec3& rhs) const
	{
		return Vec3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	float MagSqr() const
	{
		return Dot(*this);
	}

	float Mag() const
	{
		float magSqr = MagSqr();
		return (magSqr > 1e-5f) ? sqrtf(magSqr) : 0.f;
	}

	Vec3 GetNormalised() const
	{
		return *this / Mag();
	}

	static void GetAxisAndAngleBetween(Vec3& axis, float& angle, const Vec3& a, const Vec3& b)
	{
		Vec3 an = a.GetNormalised();
		Vec3 bn = b.GetNormalised();
		angle = acosf(an.Dot(bn));
		axis = an.Cross(bn);
	}
};


struct Mat44
{
	float m[4][4];

	Mat44 operator*(const Mat44& rhs)
	{
		Mat44 ret;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				float o = 0.f;
				for (int k = 0; k < 4; ++k)
				{
					o += m[i][k] * rhs.m[k][j];
				}
				ret.m[i][j] = o;
			}
		}
		return ret;
	}

	Vec3 operator*(const Vec3& rhs)
	{
		Vec3 ret(0.f, 0.f, 0.f);
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				ret[i] += m[i][j] * rhs[j];
			}
			ret[i] += m[i][3];
		}
	}

	// from http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix,
	// in turn from http://www.mesa3d.org/. Modified slightly to fit this struct.
	Mat44 GetInverse()
	{
		Mat44 outMat;
		const float* m = *this->m;
		float* invOut = *outMat.m;

		float inv[16], det;
		int i;

		inv[0] = m[5] * m[10] * m[15] -
			m[5] * m[11] * m[14] -
			m[9] * m[6] * m[15] +
			m[9] * m[7] * m[14] +
			m[13] * m[6] * m[11] -
			m[13] * m[7] * m[10];

		inv[4] = -m[4] * m[10] * m[15] +
			m[4] * m[11] * m[14] +
			m[8] * m[6] * m[15] -
			m[8] * m[7] * m[14] -
			m[12] * m[6] * m[11] +
			m[12] * m[7] * m[10];

		inv[8] = m[4] * m[9] * m[15] -
			m[4] * m[11] * m[13] -
			m[8] * m[5] * m[15] +
			m[8] * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

		inv[12] = -m[4] * m[9] * m[14] +
			m[4] * m[10] * m[13] +
			m[8] * m[5] * m[14] -
			m[8] * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

		inv[1] = -m[1] * m[10] * m[15] +
			m[1] * m[11] * m[14] +
			m[9] * m[2] * m[15] -
			m[9] * m[3] * m[14] -
			m[13] * m[2] * m[11] +
			m[13] * m[3] * m[10];

		inv[5] = m[0] * m[10] * m[15] -
			m[0] * m[11] * m[14] -
			m[8] * m[2] * m[15] +
			m[8] * m[3] * m[14] +
			m[12] * m[2] * m[11] -
			m[12] * m[3] * m[10];

		inv[9] = -m[0] * m[9] * m[15] +
			m[0] * m[11] * m[13] +
			m[8] * m[1] * m[15] -
			m[8] * m[3] * m[13] -
			m[12] * m[1] * m[11] +
			m[12] * m[3] * m[9];

		inv[13] = m[0] * m[9] * m[14] -
			m[0] * m[10] * m[13] -
			m[8] * m[1] * m[14] +
			m[8] * m[2] * m[13] +
			m[12] * m[1] * m[10] -
			m[12] * m[2] * m[9];

		inv[2] = m[1] * m[6] * m[15] -
			m[1] * m[7] * m[14] -
			m[5] * m[2] * m[15] +
			m[5] * m[3] * m[14] +
			m[13] * m[2] * m[7] -
			m[13] * m[3] * m[6];

		inv[6] = -m[0] * m[6] * m[15] +
			m[0] * m[7] * m[14] +
			m[4] * m[2] * m[15] -
			m[4] * m[3] * m[14] -
			m[12] * m[2] * m[7] +
			m[12] * m[3] * m[6];

		inv[10] = m[0] * m[5] * m[15] -
			m[0] * m[7] * m[13] -
			m[4] * m[1] * m[15] +
			m[4] * m[3] * m[13] +
			m[12] * m[1] * m[7] -
			m[12] * m[3] * m[5];

		inv[14] = -m[0] * m[5] * m[14] +
			m[0] * m[6] * m[13] +
			m[4] * m[1] * m[14] -
			m[4] * m[2] * m[13] -
			m[12] * m[1] * m[6] +
			m[12] * m[2] * m[5];

		inv[3] = -m[1] * m[6] * m[11] +
			m[1] * m[7] * m[10] +
			m[5] * m[2] * m[11] -
			m[5] * m[3] * m[10] -
			m[9] * m[2] * m[7] +
			m[9] * m[3] * m[6];

		inv[7] = m[0] * m[6] * m[11] -
			m[0] * m[7] * m[10] -
			m[4] * m[2] * m[11] +
			m[4] * m[3] * m[10] +
			m[8] * m[2] * m[7] -
			m[8] * m[3] * m[6];

		inv[11] = -m[0] * m[5] * m[11] +
			m[0] * m[7] * m[9] +
			m[4] * m[1] * m[11] -
			m[4] * m[3] * m[9] -
			m[8] * m[1] * m[7] +
			m[8] * m[3] * m[5];

		inv[15] = m[0] * m[5] * m[10] -
			m[0] * m[6] * m[9] -
			m[4] * m[1] * m[10] +
			m[4] * m[2] * m[9] +
			m[8] * m[1] * m[6] -
			m[8] * m[2] * m[5];

		det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

		if (det == 0)
			return outMat;

		det = 1.0f / det;

		for (i = 0; i < 16; i++)
			invOut[i] = inv[i] * det;

		return outMat;
	}
};

static void RenderPoint(GLUquadric& quadric, const Vec3& point, float radius)
{
	glPushMatrix();
	glTranslatef(point.x, point.y, point.z);
	gluSphere(&quadric, radius, 12, 12);
	glPopMatrix();
}

class Particle
{
public:
	Particle(Vec3 pos, float mass, Particle* prev)
		: m_pos(pos)
		, m_vel(0.f, 0.f, 0.f)
		, m_mass(mass)
		, m_invMass(mass ? 1.f / mass : 0.f)
		, m_dist(prev ? (pos - prev->m_pos).Mag() : 0.f)
	{
		if (prev)
		{
			assert(m_dist > 0.f);
		}
	}

	Vec3 m_pos;
	Vec3 m_vel;
	float m_mass;
	float m_invMass;
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
			if (p.m_invMass > 0.f)
			{
				p.m_vel += gravity * dt;
				p.m_pos += p.m_vel * dt;
			}
		}

	//	if (!m_particles.empty())
	//	{
	//		m_particles.front().m_pos = Vec3(0.f, 3.f, 0.f);
	//		m_particles.front().m_vel = Vec3(0.f, 0.f, 0.f);
	//	}
	}

	void SolveConstraints(float dt)
	{
		static const int numIterations = 100;
		for (int iter = 0; iter < numIterations; ++iter)
		{
			for (size_t partIndex = 1; partIndex < m_particles.size(); ++partIndex)
			{
				Particle& pa = m_particles[partIndex];
				Particle& pb = m_particles[partIndex - 1];
				Vec3 delta = pa.m_pos - pb.m_pos;
				float myDist = delta.Mag();
				float denom = pa.m_invMass + pb.m_invMass;
				if (denom > FLT_EPSILON)
				{
					// Find desired relvel for these particles
				//	float maxImpulse = -k * displacement * dt;

				
					// Use this to solve?

					//if (myDist > pa.m_dist)
					{
						static /*const*/ float k = 500.f;
						static /*const*/ float b = 0.f;

						float displacement = myDist - pa.m_dist;

						// Work out what the displacement will be in the next frame
						Vec3 newPosA = pa.m_pos + pa.m_vel * dt;
						Vec3 newPosB = pb.m_pos + pb.m_vel * dt;
						float newDisplacement = (newPosA - newPosB).Mag() - pa.m_dist;

						Vec3 n = delta.GetNormalised();
						Vec3 relVel = pa.m_vel - pb.m_vel;
						Vec3 impulse = (-n * k * newDisplacement - relVel * b) * dt;
						float i = impulse.Mag();
						static float breakp = 10.f;
						if (i > breakp)
						{
							int b = 0;
						}


						pa.m_vel += impulse * (pa.m_invMass / denom);
						pb.m_vel -= impulse * (pb.m_invMass / denom);
						//particle.m_pos = prevParticle.m_pos + n * particle.m_dist;
					}
				}
			}
		}
	}

	void Render(GLUquadric& quadric) const
	{
		if (!m_particles.empty())
		{
		//	glLoadIdentity();
		//	glBegin(GL_LINE_STRIP);
		//	//glBegin(GL_TRIANGLES);
		//	for (const Particle& p : m_particles)
		//	{
		//		glVertex3fv(reinterpret_cast<const GLfloat*>(&p.m_pos));
		//	}
		//	glEnd();

			for (const Particle& p : m_particles)
			{
				RenderPoint(quadric, p.m_pos, 0.1f);
			}

			for (size_t i = 1; i < m_particles.size(); ++i)
			{
				const Particle& pa = m_particles[i];
				const Particle& pb = m_particles[i - 1];

				Vec3 axis(0.f, 0.f, 0.f);
				float angle = 0.f;
				Vec3::GetAxisAndAngleBetween(axis, angle, Vec3(0.f, 0.f, 1.f), pb.m_pos - pa.m_pos);

				glPushMatrix();
				glTranslatef(pa.m_pos.x, pa.m_pos.y, pa.m_pos.z);
				glRotatef(angle * 180.f / 3.1415f, axis.x, axis.y, axis.z);
				gluCylinder(&quadric, 0.1, 0.1, (pa.m_pos - pb.m_pos).Mag(), 12, 12);
				glPopMatrix();
			}
		}
	}

	void AddParticle(const Vec3& pos, float mass)
	{
		Particle* prev = m_particles.empty() ? NULL : &m_particles.back();
		m_particles.push_back(Particle(pos, mass, prev));
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
			rope.SolveConstraints(timeStep);
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
	rope.AddParticle(startPos + Vec3(static_cast<float>(numParticles)* separation, 0.f, 0.f), 5.f);

	Integrator integrator;

	//bool isMousePressed = false;

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

		//	if (event.type == sf::Event::MouseButtonPressed)
		//	{
		//		if (event.mouseButton.button == sf::Mouse::Left)
		//		{
		//			isMousePressed = true;
		//		}
		//	}
		//
		//	if (event.type == sf::Event::MouseButtonReleased)
		//	{
		//		if (event.mouseButton.button == sf::Mouse::Left)
		//		{
		//			isMousePressed = false;
		//		}
		//	}
		}

		// Clear the depth buffer
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// Set the camera matrix.
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPushMatrix();
		Vec3 camPos = { 0.f, 0.f, -5.f };
		Vec3 camLookAt = { 0.f, 0.f, 0.f };

		gluLookAt(
			camPos.x, camPos.y, camPos.z,
			camLookAt.x, camLookAt.y, camLookAt.z,
			0.f, 1.f, 0.f);

		// TODO: tidy		
		// Project the mouse position onto the plane
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		Vec3 mousePos3(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y), 0.f);

		/*
		float partProj = (rope.m_particles.back().m_pos - camPos).Dot(lookDir);

		Mat44 modelMtx;
		glGetFloatv(GL_MODELVIEW_MATRIX, *modelMtx.m);

		Mat44 projMtx;
		glGetFloatv(GL_PROJECTION_MATRIX, *projMtx.m);
		
		Mat44 combinedMtxs = projMtx * modelMtx;
		Mat44 invCombinedMtxs = combinedMtxs.GetInverse();
		
		// Get the mouse pos on the front clip plane
		Vec3 mousePosWorld = invCombinedMtxs * mousePos3;
		// Get a vector from the camera pos to this mouse pos
		Vec3 rayDir = (mousePosWorld - camPos).GetNormalised();

		Vec3 castPoint = camPos + lookDir * partProj;
		RenderPoint(*quadric, castPoint, 0.2f);
		*/

		{
			double modelMtx[16];
			glGetDoublev(GL_MODELVIEW_MATRIX, modelMtx);

			double projMtx[16];
			glGetDoublev(GL_PROJECTION_MATRIX, projMtx);

			int viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);

			//	Vec3 ropePos = rope.m_particles.back().m_pos;
			//	double ropeProjX, ropeProjY, ropeProjZ;
			//	gluProject(ropePos.x, ropePos.y, ropePos.z,
			//		modelMtx, projMtx, viewport,
			//		&ropeProjX, &ropeProjY, &ropeProjZ);

			double castX, castY, castZ;
			gluUnProject(static_cast<float>(mousePos.x), static_cast<float>(window.getSize().y - mousePos.y), 0.f,
				modelMtx, projMtx, viewport,
				&castX, &castY, &castZ);

			Vec3 mousePosFrontClipPlane(static_cast<float>(castX), static_cast<float>(castY), static_cast<float>(castZ));
			Vec3 rayDir = (mousePosFrontClipPlane - camPos).GetNormalised();
			Vec3 lookDir = (camLookAt - camPos).GetNormalised();

			// Cast the mouse onto a plane around the last particle of the rope, with a normal of the camera forward vector (i.e. grab at a fixed distance)
			float t = (rope.m_particles.back().m_pos - mousePosFrontClipPlane).Dot(lookDir) / rayDir.Dot(lookDir);
			Vec3 castPoint = mousePosFrontClipPlane + rayDir * t;
			RenderPoint(*quadric, castPoint, 0.1f);

			bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

			// Make sure we don't exceed the window bounds
			if (0 > mousePos.x && static_cast<unsigned int>(mousePos.x) >= window.getSize().x ||
				0 > mousePos.y && static_cast<unsigned int>(mousePos.y) >= window.getSize().y)
			{
				isMousePressed = false;
			}

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
		}

		integrator.Integrate(rope, dt);


		glMatrixMode(GL_MODELVIEW);
		
		unsigned int red = 0xff0000ff;
		glColor4ubv((const GLubyte*)&red);

		rope.Render(*quadric);

		// Finally, display the rendered frame on screen
		window.display();
	}

	gluDeleteQuadric(quadric);

	return 0;
}