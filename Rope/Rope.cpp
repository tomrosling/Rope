#include "Rope.h"
#include "DebugRender.h"
#include <cassert>

Particle::Particle(const Vec3& pos, float mass, Particle* prev)
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

///////////////////////////////////////////////////////////////////////////

void Rope::Integrate(float dt)
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

void Rope::SolveConstraints(float dt)
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

void Rope::Render(GLUquadric& quadric) const
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
			DebugRender::Sphere(quadric, p.m_pos, 0.1f);
		}

		for (size_t i = 1; i < m_particles.size(); ++i)
		{
			const Particle& pa = m_particles[i];
			const Particle& pb = m_particles[i - 1];
			DebugRender::Cylinder(quadric, pa.m_pos, pb.m_pos, 0.1f);
		}
	}
}

void Rope::AddParticle(const Vec3& pos, float mass)
{
	Particle* prev = m_particles.empty() ? NULL : &m_particles.back();
	m_particles.push_back(Particle(pos, mass, prev));
}
