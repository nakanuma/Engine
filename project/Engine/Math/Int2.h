#pragma once

#include <stdint.h>

struct Int2
{
	int32_t x,y;
	 // コンストラクタ
	Int2(): x(0),y(0) {}
	Int2(int32_t x,int32_t y): x(x),y(y) {}
	Int2(const int32_t* p):x(p[0]),y(p[1]) {}
	  // 代入演算子
	Int2& operator=(const Int2& rhs)
	{
		if(this != &rhs)
		{
			this->x = rhs.x;
			this->y = rhs.y;
		}
		return *this;
	}

	// 加算演算子
	Int2 operator+(const Int2& rhs) const
	{
		return Int2(x + rhs.x,y + rhs.y);
	}
	// 加算代入演算子
	Int2& operator+=(const Int2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	// 減算演算子
	Int2 operator-(const Int2& rhs) const
	{
		return Int2(x - rhs.x,y - rhs.y);
	}
	// 減算代入演算子
	Int2& operator-=(const Int2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	// スカラー乗算演算子
	Int2 operator*(int32_t scalar) const
	{
		return Int2(x * scalar,y * scalar);
	}
	// スカラー乗算代入演算子
	Int2& operator*=(int32_t scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	// スカラー除算演算子
	Int2 operator/(int32_t scalar) const
	{
		return Int2(x / scalar,y / scalar);
	}
	// スカラー除算代入演算子
	Int2& operator/=(int32_t scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	// 比較演算子 ==
	bool operator==(const Int2& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	// 比較演算子 !=
	bool operator!=(const Int2& rhs) const
	{
		return !(*this == rhs);
	}
};