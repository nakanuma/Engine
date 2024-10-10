#pragma once


struct Float2
{
    float x;
    float y;

    // コンストラクタ
    Float2(): x(0),y(0) {}
    Float2(float x,float y): x(x),y(y) {}

    // 加算
    Float2 operator+(const Float2& rhs) const
    {
        return Float2(x + rhs.x,y + rhs.y);
    }

    // 減算
    Float2 operator-(const Float2& rhs) const
    {
        return Float2(x - rhs.x,y - rhs.y);
    }

    // スカラー乗算
    Float2 operator*(float scalar) const
    {
        return Float2(x * scalar,y * scalar);
    }

    // スカラー除算
    Float2 operator/(float scalar) const
    {
        return Float2(x / scalar,y / scalar);
    }

    // 単項マイナス
    Float2 operator-() const
    {
        return Float2(-x,-y);
    }

    // 加算代入
    Float2& operator+=(const Float2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    // 減算代入
    Float2& operator-=(const Float2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    // スカラー乗算代入
    Float2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // スカラー除算代入
    Float2& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // 等値比較
    bool operator==(const Float2& rhs) const
    {
        return (x == rhs.x) && (y == rhs.y);
    }

    // 非等値比較
    bool operator!=(const Float2& rhs) const
    {
        return !(*this == rhs);
    }

   
    // ベクトルの長さ（大きさ）を計算する関数
    static float Length(const Float2& f);

    // ベクトルを正規化する関数
    static Float2 Normalize(const Float2& f);
};

// スカラー乗算（左辺がスカラーの場合）
inline Float2 operator*(float scalar,const Float2& v)
{
    return v * scalar;
}