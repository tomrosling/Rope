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


struct Mat44
{
	float m[4][4];

	Mat44 operator*(const Mat44& rhs)
	{
		Mat44 ret;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				float o = 0.f;
				for (int k = 0; k < 4; ++k)
				{
					o += m[i][k] * rhs.m[k][j];
				}
				ret.m[i][j] = o;
			}
		}
		return ret;
	}

	Vec3 operator*(const Vec3& rhs)
	{
		Vec3 ret(0.f, 0.f, 0.f);
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				ret[i] += m[i][j] * rhs[j];
			}
			ret[i] += m[i][3];
		}
	}

	// from http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix,
	// in turn from http://www.mesa3d.org/. Modified slightly to fit this struct.
	Mat44 GetInverse()
	{
		Mat44 outMat;
		const float* m = *this->m;
		float* invOut = *outMat.m;

		float inv[16], det;
		int i;

		inv[0] = m[5] * m[10] * m[15] -
			m[5] * m[11] * m[14] -
			m[9] * m[6] * m[15] +
			m[9] * m[7] * m[14] +
			m[13] * m[6] * m[11] -
			m[13] * m[7] * m[10];

		inv[4] = -m[4] * m[10] * m[15] +
			m[4] * m[11] * m[14] +
			m[8] * m[6] * m[15] -
			m[8] * m[7] * m[14] -
			m[12] * m[6] * m[11] +
			m[12] * m[7] * m[10];

		inv[8] = m[4] * m[9] * m[15] -
			m[4] * m[11] * m[13] -
			m[8] * m[5] * m[15] +
			m[8] * m[7] * m[13] +
			m[12] * m[5] * m[11] -
			m[12] * m[7] * m[9];

		inv[12] = -m[4] * m[9] * m[14] +
			m[4] * m[10] * m[13] +
			m[8] * m[5] * m[14] -
			m[8] * m[6] * m[13] -
			m[12] * m[5] * m[10] +
			m[12] * m[6] * m[9];

		inv[1] = -m[1] * m[10] * m[15] +
			m[1] * m[11] * m[14] +
			m[9] * m[2] * m[15] -
			m[9] * m[3] * m[14] -
			m[13] * m[2] * m[11] +
			m[13] * m[3] * m[10];

		inv[5] = m[0] * m[10] * m[15] -
			m[0] * m[11] * m[14] -
			m[8] * m[2] * m[15] +
			m[8] * m[3] * m[14] +
			m[12] * m[2] * m[11] -
			m[12] * m[3] * m[10];

		inv[9] = -m[0] * m[9] * m[15] +
			m[0] * m[11] * m[13] +
			m[8] * m[1] * m[15] -
			m[8] * m[3] * m[13] -
			m[12] * m[1] * m[11] +
			m[12] * m[3] * m[9];

		inv[13] = m[0] * m[9] * m[14] -
			m[0] * m[10] * m[13] -
			m[8] * m[1] * m[14] +
			m[8] * m[2] * m[13] +
			m[12] * m[1] * m[10] -
			m[12] * m[2] * m[9];

		inv[2] = m[1] * m[6] * m[15] -
			m[1] * m[7] * m[14] -
			m[5] * m[2] * m[15] +
			m[5] * m[3] * m[14] +
			m[13] * m[2] * m[7] -
			m[13] * m[3] * m[6];

		inv[6] = -m[0] * m[6] * m[15] +
			m[0] * m[7] * m[14] +
			m[4] * m[2] * m[15] -
			m[4] * m[3] * m[14] -
			m[12] * m[2] * m[7] +
			m[12] * m[3] * m[6];

		inv[10] = m[0] * m[5] * m[15] -
			m[0] * m[7] * m[13] -
			m[4] * m[1] * m[15] +
			m[4] * m[3] * m[13] +
			m[12] * m[1] * m[7] -
			m[12] * m[3] * m[5];

		inv[14] = -m[0] * m[5] * m[14] +
			m[0] * m[6] * m[13] +
			m[4] * m[1] * m[14] -
			m[4] * m[2] * m[13] -
			m[12] * m[1] * m[6] +
			m[12] * m[2] * m[5];

		inv[3] = -m[1] * m[6] * m[11] +
			m[1] * m[7] * m[10] +
			m[5] * m[2] * m[11] -
			m[5] * m[3] * m[10] -
			m[9] * m[2] * m[7] +
			m[9] * m[3] * m[6];

		inv[7] = m[0] * m[6] * m[11] -
			m[0] * m[7] * m[10] -
			m[4] * m[2] * m[11] +
			m[4] * m[3] * m[10] +
			m[8] * m[2] * m[7] -
			m[8] * m[3] * m[6];

		inv[11] = -m[0] * m[5] * m[11] +
			m[0] * m[7] * m[9] +
			m[4] * m[1] * m[11] -
			m[4] * m[3] * m[9] -
			m[8] * m[1] * m[7] +
			m[8] * m[3] * m[5];

		inv[15] = m[0] * m[5] * m[10] -
			m[0] * m[6] * m[9] -
			m[4] * m[1] * m[10] +
			m[4] * m[2] * m[9] +
			m[8] * m[1] * m[6] -
			m[8] * m[2] * m[5];

		det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

		if (det == 0)
			return outMat;

		det = 1.0f / det;

		for (i = 0; i < 16; i++)
			invOut[i] = inv[i] * det;

		return outMat;
	}
};

template<typename T>
T Clamp(T x, T min, T max)
{
	return (x < min) ? min : ((x > max) ? max : x);
}
