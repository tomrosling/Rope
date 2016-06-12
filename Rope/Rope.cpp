#include "Rope.h"
#include <cassert>
#include <SFML/Graphics/Color.hpp>
#include "DebugRender.h"

Particle::Particle(const Vec3& pos, float mass, Particle* prev)
	: m_pos(pos)
	, m_prevPos(pos)
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
			static const float damping = 0.1f;
			Vec3 deltaPos = p.m_pos - p.m_prevPos;
			deltaPos *= expf(-damping * dt);
			p.m_prevPos = p.m_pos;
			p.m_pos += gravity * dt * dt;
			p.m_pos += deltaPos;
		}
	}
}

void Rope::SolveConstraints(float dt)
{
	static const int numIterations = 10;
	for (int iter = 0; iter < numIterations; ++iter)
	{
		for (size_t partIndex = 1; partIndex < m_particles.size(); ++partIndex)
		{
			Particle& pa = m_particles[partIndex];
			Particle& pb = m_particles[partIndex - 1];
			Vec3 delta = pa.m_pos - pb.m_pos;
			float denom = pa.m_invMass + pb.m_invMass;
			if (denom > FLT_EPSILON)
			{
				Vec3 delta = pa.m_pos - pb.m_pos;
				float deltaMag = delta.Mag();
				Vec3 deltaNorm = delta / deltaMag;
				float displacement = deltaMag - pa.m_dist;

				Vec3 impulse = -deltaNorm * displacement;
				pa.m_pos += impulse * (pa.m_invMass / denom);
				pb.m_pos -= impulse * (pb.m_invMass / denom);
			}
		}
	}
}

void Rope::Render(GLUquadric& quadric) const
{
	if (!m_particles.empty())
	{
		static const float radius = 0.02f;
		static const sf::Color brown(139, 69, 19);

		for (const Particle& p : m_particles)
		{
			//float radius;
			//if (p.m_invMass > FLT_EPSILON)
			//{
			//	float mass = 1.f / p.m_invMass;
			//	static const float density = 100.f;
			//	static const float PI = 3.14159265f;
			//	radius = powf((3.f * mass) / (4.f * PI * density), 1.f / 3.f);
			//}
			//else
			//{
			//	radius = 0.1f;
			//}
		
			DebugRender::Sphere(quadric, p.m_pos, radius, brown);
		}

		for (size_t i = 1; i < m_particles.size(); ++i)
		{
			const Particle& pa = m_particles[i];
			const Particle& pb = m_particles[i - 1];
			DebugRender::Cylinder(quadric, pa.m_pos, pb.m_pos, radius, brown);
		}
	}
}

void Rope::AddParticle(const Vec3& pos, float mass)
{
	Particle* prev = m_particles.empty() ? NULL : &m_particles.back();
	m_particles.push_back(Particle(pos, mass, prev));
}
