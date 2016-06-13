#include "DebugRender.h"
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Color.hpp>
#include <gl/GLU.h>
#include <cassert>
#include "Maths.h"

GLUquadric* DebugRender::s_quadric = NULL;

void DebugRender::Initialise(int windowSizeX, int windowSizeY)
{
	// Set up the Z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	// Set up lighting
	GLfloat	globalAmbient[] = { 0.3f, 0.3f, 0.3f, 1.f };
	GLfloat light0Diff[] = { 0.9f, 0.9f, 0.7f, 1.f };
	GLfloat light0Pos[] = { 0.f, 0.f, 5.f, 1.0 };
	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diff);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glEnable(GL_LIGHT0);

	// Setup the viewport and perspective projection
	glViewport(0, 0, windowSizeX, windowSizeY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = static_cast<float>(windowSizeX) / static_cast<float>(windowSizeY);
	gluPerspective(75.f, ratio, 1.f, 50.f);

	// Create a quadrics object for glu rendering
	s_quadric = gluNewQuadric();
	assert(s_quadric);
}

void DebugRender::Shutdown()
{
	gluDeleteQuadric(s_quadric);
}

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

void DebugRender::Sphere(const Vec3& point, float radius, const sf::Color& colour)
{
	SetColour(colour);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(point.x, point.y, point.z);
	gluSphere(s_quadric, radius, 12, 12);
	glPopMatrix();
}

void DebugRender::Cylinder(const Vec3& a, const Vec3& b, float radius, const sf::Color& colour)
{
	SetColour(colour);

	Vec3 delta = b - a;
	Vec3 axis(0.f, 0.f, 0.f);
	float angle = 0.f;
	Vec3::GetAxisAndAngleBetween(axis, angle, Vec3(0.f, 0.f, 1.f), b - a);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(a.x, a.y, a.z);
	glRotatef(angle * 180.f / PI, axis.x, axis.y, axis.z);
	gluCylinder(s_quadric, radius, radius, delta.Mag(), 12, 12);
	glPopMatrix();
}

void DebugRender::SetColour(const sf::Color& colour)
{
	GLfloat colourGL[] = { static_cast<float>(colour.r) / 255.f, static_cast<float>(colour.g) / 255.f, static_cast<float>(colour.b) / 255.f, 1.f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colourGL);
}
