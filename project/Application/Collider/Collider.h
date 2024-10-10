#pragma once
#pragma once

#include <functional>
#include "Object3D.h"
#include "ModelManager.h"
#include "Float3.h"
#include <stdint.h>

class Collider {
public:
	Collider() = default;
	void Init(const Float3& pos, float radius, std::function<void(Collider* a)> onCollisionFunc);
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
	bool getIsInvincible() const { return invincibleTime_ > 0; }
	void setInvincibleTime(const float& time) { invincibleTime_ = time; }

	Float3 getPosition() const { return object.transform_.translate; }

	float getRadius() const { return radius_; }
	void setTransformParent(Object3D* parent) {
		object.SetParent(parent);
		object.UpdateMatrix();
	}

	uint32_t getTypeID() const { return typeID_; }
	void setTypeID(uint32_t typeID) { typeID_ = typeID; }


};

