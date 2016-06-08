#pragma once

class GLUquadric;
struct Vec3;

class DebugRender
{
public:
	static void Sphere(GLUquadric& quadric, const Vec3& point, float radius);
	static void Cylinder(GLUquadric& quadric, const Vec3& a, const Vec3& b, float radius);
};
