#pragma once

class Rope;

class Integrator
{
public:
	Integrator();
	void Integrate(Rope& rope, float dt);

private:
	float m_timeAccumulator;
};
