#include "EnemySpawner.h"

#include "DeltaTime.h"
#include "GlobalVariables.h"
#include "Application/Stage/Stage.h"

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
	
	controlByEnergy_ = false;
	controlByTime_ = false;
	variables->addValue("Game",group,"controlByTime_",controlByTime_);
	variables->addValue("Game",group,"controlByEnergy_",controlByEnergy_);

	variables->addValue("Game",group,"inactiveEnemyValue_",inactiveEnemyValue_);
	variables->addValue("Game",group,"inactiveMidTime_",inactiveMidTime_);
	inactiveTimeLength_ = stage_->GetLimitTime();
	variables->addValue("Game",group,"inactiveTimeLegth_",inactiveTimeLength_);
	variables->addValue("Game",group,"inactiveMidEnergy_",inactiveMidEnergy_);
	inactiveEnergyLength_ = stage_->GetMaxEnergy();
	variables->addValue("Game",group,"inactiveEnergyLength_",inactiveEnergyLength_);
}

void EnemySpawner::Update()
{
	isSpawn_ = false;


	if(inactiveEnemyValue_ <= stage_->GetEnemies().size())
	{
	productionPoint_ = 1.0f;

	if(controlByTime_)
	{
		const float currentTime = stage_->GetLeftTime();
		float timeDist = inactiveMidTime_ - currentTime;
		if(timeDist * timeDist >= inactiveTimeLength_ * inactiveTimeLength_)
		{
			productionPoint_ -= ((timeDist * timeDist) / (stage_->GetLimitTime() * stage_->GetLimitTime()));
		}
	}
	if(controlByEnergy_)
	{
		const float currentEnergy = stage_->GetChargedEnergy();
		float energyDist = inactiveMidEnergy_ - currentEnergy;
		if(energyDist * energyDist >= inactiveEnergyLength_ * inactiveEnergyLength_)
		{
			productionPoint_ -= (energyDist * energyDist) / (stage_->GetMaxEnergy() * stage_->GetMaxEnergy());
		}
		}
	} else
	{
		productionPoint_ = 0.0f;
	}

	productionPoint_ = (std::max)(productionPoint_,0.0f);
	currentCoolTime_ -= DeltaTime::getInstance()->getDeltaTime() * productionPoint_;

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
