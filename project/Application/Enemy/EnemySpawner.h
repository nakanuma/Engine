#pragma once

#include <memory>

#include "Application/MyRandom/MyRandom.h"
#include "Float3.h"
#include "ModelManager.h"
#include "Object3D.h"

#include "Enemy.h"

class Stage;
class EnemySpawner
{
public:
	EnemySpawner(Stage* stage):stage_(stage){}
	~EnemySpawner();

	void Initialize(const std::string& scene,int32_t listNum,ModelManager::ModelData* modelData);
	void Update();
	void Draw();

	Enemy* Spawn();
private:
	Stage* stage_;

	// controlByEnemyValue
	bool controlByTime_;
	bool controlByEnergy_;

	float inactiveEnemyValue_;
	float inactiveMidTime_;
	float inactiveTimeLength_;
	float inactiveMidEnergy_;
	float inactiveEnergyLength_;

	float productionPoint_ = 1.0f;

	int32_t listNum_;
	Float2 moveDirection_;
	MyRandom spawnCoolTime_;
	float currentCoolTime_;

	std::unique_ptr<Object3D> object_;

	ModelManager::ModelData enemyModel_;

	bool isSpawn_ = false;
public:
	bool IsSpawn()const{return isSpawn_;}
	void SetEnemyModel(ModelManager::ModelData enemyModel){enemyModel_ = enemyModel;}
};
