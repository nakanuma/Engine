#pragma once

#include <memory>

#include <stdint.h>

#include "ModelManager.h"
#include "Object3D.h"

#include "Float2.h"
#include "Float3.h"

class Collider;
class Enemy
{
public:
	Enemy();
	~Enemy();

	void Initialize(Float3 spawnPos,Float2 moveDirection,ModelManager::ModelData* modelData);
	void Update();
	void Draw();
private:
	std::unique_ptr<Object3D> object_;
	std::unique_ptr<Collider> collider_;

	Float2 moveDirection_;
	float speed_;

	Float3 velocity_;
	float  floorVelocityY_;
	bool isAlive_;

	bool isOnGround_;
	bool preOnGround_;
public:
	bool IsAlive()const{return isAlive_;}
	Collider* GetCollider()const { return collider_.get(); }
};