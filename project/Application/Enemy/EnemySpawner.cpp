#include "EnemySpawner.h"

#include "DeltaTime.h"
#include "GlobalVariables.h"

EnemySpawner::~EnemySpawner()
{
}

void EnemySpawner::Initialize(int32_t listNum,ModelManager::ModelData* modelData)
{
	listNum_ = listNum;
	std::string group = "EnemySpawner" + std::to_string(listNum_);
	spawnCoolTime_ = MyRandom ("Game",group,"spawnCoolTime");
	currentCoolTime_ = spawnCoolTime_();
	object_ = std::make_unique<Object3D>();
	object_->model_ = modelData;

	GlobalVariables *variables = GlobalVariables::getInstance();
	variables->addValue("Game",group,"moveDirection",moveDirection_);
	moveDirection_ = Float2::Normalize(moveDirection_);
	variables->addValue("Game",group,"Translate",object_->transform_.translate);
}

void EnemySpawner::Update()
{
	isSpawn_ = false;

	currentCoolTime_ -= DeltaTime::getInstance()->getDeltaTime();
	if(currentCoolTime_ <= 0.0f)
	{
		isSpawn_ = true;
		currentCoolTime_ = spawnCoolTime_();
	}

#ifdef _DEBUG
	moveDirection_ = Float2::Normalize(moveDirection_);
#endif // _DEBUG

	object_->UpdateMatrix();
}

void EnemySpawner::Draw()
{
	object_->Draw();
}

Enemy* EnemySpawner::Spawn()
{
	Enemy* enemy = new Enemy();
	enemy->Initialize(object_->transform_.translate,moveDirection_,&enemyModel_);
	return enemy;
}
