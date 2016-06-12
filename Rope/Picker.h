#pragma once

namespace sf
{
	class Window;
}

class Rope;
struct Vec3;

class Picker
{
public:
	Picker();
	void Pick(const sf::Window& window, const Vec3& camPos, const Vec3& camLookAt, Rope& rope);

private:
	float m_planeW;
};