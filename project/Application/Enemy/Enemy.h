#pragma once

#include <memory>

#include <stdint.h>

#include "Application/Collision/Collider.h"
#include "ModelManager.h"
#include "Object3D.h"

#include "Float2.h"
#include "Float3.h"

class Enemy
{
public:
	Enemy() = default;
	~Enemy();

	void Initialize(ModelManager::ModelData* modelData);
	void Update();
	void Draw();

	void OnCollisionMapChip(const Float2& dir,float chipY,float mapChipVelocityY);
private:
	std::unique_ptr<Object3D> object_;
	std::unique_ptr<Collider> collider_;

	Float2 moveDirection_;
	float speed_;

	Float3 velocity_;
	float floorVelocityY_;

	bool isOnGround_;
	bool preOnGround_;

public:
	Collider* GetCollider()const { return collider_.get(); }
};