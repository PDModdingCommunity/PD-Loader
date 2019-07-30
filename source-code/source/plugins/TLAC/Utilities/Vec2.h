#pragma once

namespace TLAC::Utilities
{
	struct Vec2
	{
		float X, Y;

		Vec2();
		Vec2(float x, float y);

		Vec2 operator+(Vec2 value);
		void operator+=(const Vec2 &value);

		Vec2 operator-(Vec2 value);
		void operator-=(const Vec2 &value);
	};
}
