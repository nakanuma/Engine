#include "Float2.h"

#include <cmath>
#include "MyMath.h"

float Float2::Length(const Float2& f)
{
	return std::sqrt(f.x * f.x + f.y * f.y);
}

Float2 Float2::Normalize(const Float2& f)
{
	float length = Length(f);

	if(length == 0.0f)
	{
		return Float2(0,0);  // 長さ0のベクトルのまま
	}
	return Float2(f.x / length,f.y / length);
}

Float2 Float2::Lerp(float t,const Float2& start,const Float2& end)
{
	return {
		start.x * (1.0f - t) + end.x * t,
		start.y * (1.0f - t) + end.y * t
	};
}