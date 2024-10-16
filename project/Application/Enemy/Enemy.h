#pragma once

#include <memory>
#include <list>

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
	void Update(std::list<std::unique_ptr<Enemy>>& enemies);
	void Draw();
private:
	void CloneInitialize(Float3 spawnPos,Float2 moveDirection,ModelManager::ModelData* modelData);
	Enemy* CreateClone();
private:
	bool isAlive_;
	std::unique_ptr<Object3D> object_;
	std::unique_ptr<Collider> collider_;

	Float2 moveDirection_;
	float speed_;
	Float3 velocity_;

	// Wave / Jump
	float mapObjectWaveDistance_;
	float waveRange_;

	float minJumpPower_;
	float maxJumpPower_;
	
	bool onWavingMapChip_;
	bool isOnGround_;
	bool preOnGround_;

	// Clone
	bool isClone_;
	Float3 cloneOffset_;
public:
	bool IsAlive()const{return isAlive_;}
	Collider* GetCollider()const { return collider_.get(); }
};