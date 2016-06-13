#pragma once
#include <vector>
#include "Maths.h"

struct Particle
{
public:
	Particle(const Vec3& pos, float mass, Particle* prev);

	Vec3 m_pos;
	Vec3 m_prevPos;
	float m_invMass;
	float m_dist;
};


class Rope
{
public:
	void IntegratePosVel(float dt);
	void SolveConstraints();
	void Render() const;
	void AddParticle(const Vec3& pos, float mass);
	Particle* GetLastParticle();

private:
	std::vector<Particle> m_particles;
};