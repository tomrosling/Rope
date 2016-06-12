#include "DebugRender.h"
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Color.hpp>
#include <gl/GLU.h>
#include "Maths.h"

void DebugRender::Line(const Vec3& a, const Vec3& b, const sf::Color& colour)
{
	SetColour(colour);

	glBegin(GL_LINES);
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, b.y, b.z);
	glEnd();
}

void DebugRender::Quad(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const sf::Color& colour)
{
	SetColour(colour);

	glBegin(GL_TRIANGLES);
	glVertex3f(a.x, a.y, a.z);
	glVertex3f(b.x, b.y, b.z);
	glVertex3f(c.x, c.y, c.z);
	glVertex3f(c.x, c.y, c.z);
	glVertex3f(d.x, d.y, d.z);
	glVertex3f(a.x, a.y, a.z);
	glEnd();
}

void DebugRender::Sphere(GLUquadric& quadric, const Vec3& point, float radius, const sf::Color& colour)
{
	SetColour(colour);

	glPushMatrix();
	glTranslatef(point.x, point.y, point.z);
	gluSphere(&quadric, radius, 12, 12);
	glPopMatrix();
}

void DebugRender::Cylinder(GLUquadric& quadric, const Vec3& a, const Vec3& b, float radius, const sf::Color& colour)
{
	SetColour(colour);

	Vec3 delta = b - a;
	Vec3 axis(0.f, 0.f, 0.f);
	float angle = 0.f;
	Vec3::GetAxisAndAngleBetween(axis, angle, Vec3(0.f, 0.f, 1.f), b - a);

	glPushMatrix();
	glTranslatef(a.x, a.y, a.z);
	glRotatef(angle * 180.f / PI, axis.x, axis.y, axis.z);
	gluCylinder(&quadric, radius, radius, delta.Mag(), 12, 12);
	glPopMatrix();
}

void DebugRender::SetColour(const sf::Color& colour)
{
	GLfloat colourGL[] = { static_cast<float>(colour.r) / 255.f, static_cast<float>(colour.g) / 255.f, static_cast<float>(colour.b) / 255.f, 1.f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colourGL);
}