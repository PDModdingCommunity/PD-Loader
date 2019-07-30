#pragma once
#include <cmath>
#include "Vec2.h"
#include "Vec3.h"

namespace TLAC::Utilities
{
	float ToDegrees(float radians);
	float ToRadians(float degrees);

	Vec2 GetDirection(float degrees);
	Vec2 PointFromAngle(float degrees, float distance);
	float AngleFromPoints(Vec2 p0, Vec2 p1);
	float ConvertRange(float originalStart, float originalEnd, float newStart, float newEnd, float value);
}