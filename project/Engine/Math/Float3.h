#pragma once
#include <cmath>
#include <cassert>

class Matrix;

struct Float3
{
	float x;
	float y;
	float z;

    ///
    /// 二項演算子のオーバーロード
    /// 

    // 加算
	Float3 operator+(const Float3& other) const;
    // 減算
	Float3 operator-(const Float3& other) const;
    // 右辺のスカラーと乗算
	Float3 operator*(float scalar) const;
    // 左辺のスカラーと乗算
	friend Float3 operator*(float scalar, const Float3& vec);

    ///
    /// 複合代入演算子のオーバーロード
    /// 

    Float3& operator+=(const Float3& other);
	Float3& operator-=(const Float3& other);

    // 長さ（ノルム）
	static float Length(const Float3& v);
    // 正規化
	static Float3 Normalize(const Float3& a);
    // 線形補間
	static Float3 Lerp(const Float3& a, const Float3& b, float t);


};