#include "Vec2.h"

namespace TLAC::Utilities
{
	Vec2::Vec2() 
	{
	};

	Vec2::Vec2(float x, float y) : X(x), Y(y) 
	{
	};

	Vec2 Vec2::operator+(Vec2 value)
	{
		return Vec2(X + value.X, Y + value.Y);
	}

	void Vec2::operator+=(const Vec2 &value)
	{
		X += value.X;
		Y += value.Y;
	}

	Vec2 Vec2::operator-(Vec2 value)
	{
		return Vec2(X - value.X, Y - value.Y);
	}

	void Vec2::operator-=(const Vec2 &value)
	{
		X -= value.X;
		Y -= value.Y;
	}
}