#pragma once
#include <math.h>

static const float PI = 3.14159265f;

struct Vec3
{
	float x, y, z;

	Vec3(float x, float y, float z)
		: x(x)
		, y(y)
		, z(z)
	{
	}

	Vec3 operator+(const Vec3& rhs) const
	{
		return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	Vec3 operator-(const Vec3& rhs) const
	{
		return *this + (-rhs);
	}

	Vec3 operator*(float s) const
	{
		return Vec3(s * x, s * y, s * z);
	}

	Vec3 operator/(float s) const
	{
		float invS = (s > 1e-5f) ? 1.f / s : 0.f;
		return *this * invS;
	}

	Vec3& operator+=(const Vec3& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	Vec3& operator-=(const Vec3& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	Vec3& operator*=(float s)
	{
		*this = *this * s;
		return *this;
	}

	Vec3& operator/=(float s)
	{
		*this = *this / s;
		return *this;
	}

	float operator[](int i) const
	{
		return *(&x + i);
	}

	float& operator[](int i)
	{
		return *(&x + i);
	}

	float Dot(const Vec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	Vec3 Cross(const Vec3& rhs) const
	{
		return Vec3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	float MagSqr() const
	{
		return Dot(*this);
	}

	float Mag() const
	{
		float magSqr = MagSqr();
		return (magSqr > 1e-5f) ? sqrtf(magSqr) : 0.f;
	}

	Vec3 GetNormalised() const
	{
		return *this / Mag();
	}

	static void GetAxisAndAngleBetween(Vec3& axis, float& angle, const Vec3& a, const Vec3& b)
	{
		Vec3 an = a.GetNormalised();
		Vec3 bn = b.GetNormalised();
		angle = acosf(an.Dot(bn));
		axis = an.Cross(bn);
	}
};
