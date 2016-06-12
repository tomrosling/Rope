#pragma once

#include "Maths.h"

class Camera
{
public:
	Camera();
	void Update(float dt);
	const Vec3& GetPos() const { return m_pos; }
	const Vec3& GetLookAt() const { return m_lookAt; }

private:
	float m_angle;
	Vec3 m_pos;
	Vec3 m_lookAt;
};
