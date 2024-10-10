#pragma once
#pragma once

#include <functional>
#include "Object3D.h"
#include "ModelManager.h"
#include "Float3.h"
#include <stdint.h>

class Collider
{
public:
	Collider() = default;
	void Init(const Float3& pos,float radius,[[maybe_unused]] std::function<void(Collider* a)> onCollisionFunc);
	void Update();
	void Draw();
	void OnCollision(Collider* collider) { onCollision_(collider); }
private:
	std::function<void(Collider* a)> onCollision_;

	// 半径
	float radius_;

	// オブジェクト
	Object3D object;

	// 無敵時間
	float invincibleTime_ = 0.0f;

	uint32_t typeID_ = 0u;

	// モデル
	ModelManager::ModelData model_;

public:
	bool GetIsInvincible() const { return invincibleTime_ > 0; }
	void SetInvincibleTime(const float& time) { invincibleTime_ = time; }

	Float3 GetPosition() const { return object.transform_.translate; }
	void SetPosition(const Float3& pos) { object.transform_.translate = pos; }

	float GetRadius() const { return radius_; }
	void SetTransformParent(Object3D* parent)
	{
		object.SetParent(parent);
		object.UpdateMatrix();
	}

	uint32_t GetTypeID() const { return typeID_; }
	void etTypeID(uint32_t typeID) { typeID_ = typeID; }
};