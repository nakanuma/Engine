#pragma once

#include <cstdint>

// コリジョン種別ID定義
enum class CollisionTypeIdDef : uint32_t
{
	kdefault,
	kPlayer,
	kPlayerWeapon,
	kEnemy,
};