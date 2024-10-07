#pragma once
#include "Float2.h"
#include "Float3.h"
#include "Float4.h"
#include "Matrix3x3.h"
#include "Matrix.h"
#include "Transform.h"
#include "Quaternion.h"
#include "QuaternionTransform.h"

static constexpr double PI = 3.14159265359;

static constexpr float PIf = 3.14159265359f;

// AABB
struct AABB {
    Float3 min;
    Float3 max;
};

// AABBと点の衝突判定
static bool IsCollision(const AABB& aabb, const Float3& point)
{
    // 点がAABBの範囲内にあるかどうかを判定
    if (point.x >= aabb.min.x && point.x <= aabb.max.x &&
        point.y >= aabb.min.y && point.y <= aabb.max.y &&
        point.z >= aabb.min.z && point.z <= aabb.max.z) {
        return true;
    }

    return false;
}

// 座標変換
static Float3 TransformMatrix(const Float3& vector, const Matrix& matrix) {
    Float3 result;

    result.x = vector.x * matrix.r[0][0] + vector.y * matrix.r[1][0] + vector.z * matrix.r[2][0] + 1.0f * matrix.r[3][0];
    result.y = vector.x * matrix.r[0][1] + vector.y * matrix.r[1][1] + vector.z * matrix.r[2][1] + 1.0f * matrix.r[3][1];
    result.z = vector.x * matrix.r[0][2] + vector.y * matrix.r[1][2] + vector.z * matrix.r[2][2] + 1.0f * matrix.r[3][2];
    float w = vector.x * matrix.r[0][3] + vector.y * matrix.r[1][3] + vector.z * matrix.r[2][3] + 1.0f * matrix.r[3][3];
    assert(w != 0.0f);

    result.x /= w;
    result.y /= w;
    result.z /= w;

    return result;
}

static Float3 Add(const Float3& v1, const Float3& v2) {
    Float3 result{};

    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;

    return result;
};

static Float3 Subtract(const Float3& v1, const Float3& v2) {
    Float3 result{};

    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;

    return result;
};


static bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
    if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
        (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
        (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {

        return true;
    }

    return false;
}