#include "Float2.h"

#include <cmath>


Float2 Float2::operator+(const Float2& other)
{
	return Float2(this->x + other.x,this->y + other.y);
}

Float2& Float2::operator+=(const Float2& other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}

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
