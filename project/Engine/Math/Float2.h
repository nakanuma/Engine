#pragma once


struct Float2
{
	float x;
	float y;

	Float2   operator+(const Float2& other);
	Float2& operator+=(const Float2& other);

    // ベクトルの長さ（大きさ）を計算する関数
    static float Length(const Float2& f);

    // ベクトルを正規化する関数
    static Float2 Normalize(const Float2& f);
};