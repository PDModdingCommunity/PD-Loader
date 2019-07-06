#pragma once
#include "Vec2.h"

namespace TLAC::Utilities
{
	struct Vec3
	{
		float X, Y, Z;

		Vec3();
		Vec3(float x, float y, float z);

		// Vec2
		Vec3 operator+(Vec2 value);
		void operator+=(const Vec2 &value);

		Vec3 operator-(Vec2 value);
		void operator-=(const Vec2 &value);

		// Vec3
		Vec3 operator+(Vec3 value);
		void operator+=(const Vec3 &value);

		Vec3 operator-(Vec3 value);
		void operator-=(const Vec3 &value);
	};
}