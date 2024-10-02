#include "Float3.h"

Float3 Float3::operator+(const Float3& other) const { 
	return {x + other.x, y + other.y, z + other.z}; }

Float3 Float3::operator-(const Float3& other) const { 
	return {x - other.x, y - other.y, z - other.z}; }

Float3 Float3::operator*(float scalar) const { 
	return {x * scalar, y * scalar, z * scalar}; }

Float3& Float3::operator+=(const Float3& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Float3& Float3::operator-=(const Float3& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

float Float3::Length(const Float3& v) { 
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Float3 Float3::Normalize(const Float3& a) { 
	float length = Length(a);
	return Float3(a.x / length, a.y / length, a.z / length);
}

Float3 Float3::Lerp(const Float3& a, const Float3& b, float t) { 
	return {
	    a.x * (1.0f - t) + b.x * t,
	    a.y * (1.0f - t) + b.y * t,
	    a.z * (1.0f - t) + b.z * t,
	};
}

Float3 operator*(float scalar, const Float3& vec) { 
	return {vec.x * scalar, vec.y * scalar, vec.z * scalar};
}
