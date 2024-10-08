#include "Float2.h"

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