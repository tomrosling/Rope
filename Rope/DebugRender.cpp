#include "DebugRender.h"
#include <SFML/OpenGL.hpp>
#include <gl/GLU.h>
#include "Maths.h"

void DebugRender::Sphere(GLUquadric& quadric, const Vec3& point, float radius)
{
	glPushMatrix();
	glTranslatef(point.x, point.y, point.z);
	gluSphere(&quadric, radius, 12, 12);
	glPopMatrix();
}

void DebugRender::Cylinder(GLUquadric& quadric, const Vec3& a, const Vec3& b, float radius)
{
	Vec3 delta = b - a;
	Vec3 axis(0.f, 0.f, 0.f);
	float angle = 0.f;
	Vec3::GetAxisAndAngleBetween(axis, angle, Vec3(0.f, 0.f, 1.f), b - a);

	glPushMatrix();
	glTranslatef(a.x, a.y, a.z);
	glRotatef(angle * 180.f / 3.1415f, axis.x, axis.y, axis.z);
	gluCylinder(&quadric, 0.1, 0.1, delta.Mag(), 12, 12);
	glPopMatrix();
}