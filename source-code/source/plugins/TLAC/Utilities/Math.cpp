#define _USE_MATH_DEFINES
#include "Math.h"

namespace TLAC::Utilities
{
	float ToDegrees(float radians)
	{
		return radians * (180.0f / M_PI);
	}

	float ToRadians(float degrees)
	{
		return (degrees * M_PI) / 180.0f;
	}

	Vec2 GetDirection(float degrees)
	{
		float radians = ToRadians(degrees);
		return Vec2(cos(radians), sin(radians));
	}

	Vec2 PointFromAngle(float degrees, float distance)
	{
		float radians = ToRadians(degrees + 90.0f);
		return Vec2(-1 * std::cos(radians) * distance, -1 * std::sin(radians) * distance);
	}

	float AngleFromPoints(Vec2 p0, Vec2 p1)
	{
		return (float)(std::atan2(p1.Y - p0.Y, p1.X - p0.X) * 180.0 / M_PI) + 90.0f;
	}

	float ConvertRange(float originalStart, float originalEnd, float newStart, float newEnd, float value)
	{
		return newStart + ((value - originalStart) * (newEnd - newStart) / (originalEnd - originalStart));
	}
}