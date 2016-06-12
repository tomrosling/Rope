#pragma once
#include <vector>
#include "Maths.h"

class GLUquadric;

class Particle
{
public:
	Particle(const Vec3& pos, float mass, Particle* prev);

	Vec3 m_pos;
	Vec3 m_prevPos;
	float m_mass;
	float m_invMass;
	float m_dist;
};


class Rope
{
public:
	void Integrate(float dt);
	void SolveConstraints(float dt);
	void Render(GLUquadric& quadric) const;
	void AddParticle(const Vec3& pos, float mass);

	std::vector<Particle> m_particles;
};