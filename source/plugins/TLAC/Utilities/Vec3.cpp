#include "Vec3.h"

namespace TLAC::Utilities
{
	Vec3::Vec3() 
	{
	};

	Vec3::Vec3(float x, float y, float z) : X(x), Y(y), Z(z) 
	{
	};

	Vec3 Vec3::operator+(Vec2 value)
	{
		return Vec3(X + value.X, Y, Z + value.Y);
	}

	void Vec3::operator+=(const Vec2 &value)
	{
		X += value.X;
		Z += value.Y;
	}

	Vec3 Vec3::operator-(Vec2 value)
	{
		return Vec3(X - value.X, Y, Z - value.Y);
	}

	void Vec3::operator-=(const Vec2 &value)
	{
		X -= value.X;
		Z -= value.Y;
	}

	Vec3 Vec3::operator+(Vec3 value)
	{
		return Vec3(X + value.X, Y + value.Y, Z + value.Z);
	}

	void Vec3::operator+=(const Vec3 &value)
	{
		X += value.X;
		Y += value.Y;
		Z += value.Z;
	}

	Vec3 Vec3::operator-(Vec3 value)
	{
		return Vec3(X - value.X, Y - value.Y, Z - value.Z);
	}

	void Vec3::operator-=(const Vec3 &value)
	{
		X -= value.X;
		Y -= value.Y;
		Z -= value.Z;
	}
}