#include "Integrator.h"
#include "Rope.h"

Integrator::Integrator()
	: m_timeAccumulator(0.f)
{
}

void Integrator::Integrate(Rope& rope, float dt)
{
	static const float timeStep = 1.f / 60.f;
	m_timeAccumulator += dt;
	while (m_timeAccumulator >= timeStep)
	{
		m_timeAccumulator -= timeStep;
		rope.IntegratePosVel(timeStep);
		rope.SolveConstraints(timeStep);
	}
}
