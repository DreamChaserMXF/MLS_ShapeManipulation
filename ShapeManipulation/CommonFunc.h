#ifndef COMMON_FUNC_H
#define COMMON_FUNC_H

#include "..\local_glm.h"


template<class T>
bool abs_less_than(const T& lhs, const T& rhs)
{
	return abs(lhs) < abs(rhs);
}

static bool max_val_less_than(const glm::vec2 v1, const glm::vec2 v2)
{
	if((abs(v1.x) >= abs(v2.x) && abs(v1.x) >= abs(v2.y)) || (abs(v1.y) >= abs(v2.x) && abs(v1.y) >= abs(v2.y)))
	{
		return false;
	}
	else
	{
		return true;
	}
}

template<class PointType>
typename PointType::value_type GetPointDistSquare(const PointType& p1, const PointType& p2)
{
	return (PointType::value_type)((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

template<class PointType>
PointType normalize(const PointType vec)
{
	double norm = sqrt(vec.x * vec.x + vec.y * vec.y);
	return PointType(vec.x / norm, vec.y / norm);
}

#endif