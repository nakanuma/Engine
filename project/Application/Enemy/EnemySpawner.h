#pragma once

#include <memory>

#include "Application/MyRandom/MyRandom.h"
#include "Float3.h"
#include "ModelManager.h"
#include "Object3D.h"

#include "Enemy.h"

class EnemySpawner
{
public:
	EnemySpawner() = default;
	~EnemySpawner();

	void Initialize(int32_t listNum,ModelManager::ModelData* modelData);
	void Update();
	void Draw();

	Enemy* Spawn();
private:
	int32_t listNum_;
	Float2   moveDirection_;
	MyRandom spawnCoolTime_;
	float currentCoolTime_;

	std::unique_ptr<Object3D> object_;

	ModelManager::ModelData enemyModel_;

	bool isSpawn_ = false;
public:
	bool IsSpawn()const{return isSpawn_;}
	void SetEnemyModel(ModelManager::ModelData enemyModel){enemyModel_ = enemyModel;}
};
