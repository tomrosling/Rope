#pragma once

class GLUquadric;
struct Vec3;

class DebugRender
{
public:
	static void Line(const Vec3& a, const Vec3& b, const Vec3& colour);
	static void Quad(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Vec3& colour);
	static void Sphere(GLUquadric& quadric, const Vec3& point, float radius, const Vec3& colour);
	static void Cylinder(GLUquadric& quadric, const Vec3& a, const Vec3& b, float radius, const Vec3& colour);

private:
	static void SetColour(const Vec3& colour);
};
