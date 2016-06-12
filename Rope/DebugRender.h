#pragma once

namespace sf
{
	class Color;
}

class GLUquadric;
struct Vec3;

class DebugRender
{
public:
	static void Initialise();
	static void Shutdown();

	static void Line(const Vec3& a, const Vec3& b, const sf::Color& colour);
	static void Quad(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const sf::Color& colour);
	static void Sphere(const Vec3& point, float radius, const sf::Color& colour);
	static void Cylinder(const Vec3& a, const Vec3& b, float radius, const sf::Color& colour);

private:
	static void SetColour(const sf::Color& colour);

	static GLUquadric* s_quadric;
};
