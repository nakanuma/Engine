#include "Enemy.h"

#include "Application/Collision/Collider.h"
#include "Application/myRandom/MyRandom.h"
#include "Application/Stage/Stage.h"
#include "DeltaTime.h"
#include "GlobalVariables.h"

#include "Matrix.h"
#include "MyMath.h"

#include <cmath>

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->DestroyItem("Game","Enemy","speed");
}

void Enemy::Initialize(Float3 spawnPos,Float2 moveDirection,ModelManager::ModelData* modelData)
{
	isAlive_ = true;
	isHurt_ = false;
	isClone_ = false;

	object_ = std::make_unique<Object3D>();
	object_->model_ = modelData;
	object_->transform_.translate = spawnPos;

	moveDirection_ = moveDirection;
	moveDirection_ = Float2::Normalize(moveDirection_);
	object_->transform_.rotate.y = atan2(moveDirection_.y,moveDirection_.x);

	collider_ = std::make_unique<Collider>();

	auto onCollision = []([[maybe_unused]] Collider* a) {};

	auto onCollisionMapChip = [this](MapChipField::MapObject* mapObject)
	{
		isOnGround_ = true;

		// Wave が 発動した 床
		if(mapObject->isWave && mapObject->waveDelay <= 0.0f)
		{
			// 原点からの方向
			Float2 moveDirectionXZ = Float2({
				static_cast<float>(mapObject->GetIndexSet().xIndex - mapObject->addressOfWaveOrigin_.xIndex),
				static_cast<float>(mapObject->addressOfWaveOrigin_.zIndex - mapObject->GetIndexSet().zIndex)
											});
			onWavingMapChip_ = true;

			waveRange_ = mapObject->waveRange_;

			mapObjectWaveDistance_ = Float2::Length(moveDirectionXZ);
			moveDirection_ = Float2::Normalize(moveDirectionXZ);
			object_->transform_.rotate.y = atan2(moveDirection_.y,moveDirection_.x);
		}
		object_->transform_.translate.y = mapObject->GetTranslate().y + collider_->GetRadius();
	};

	collider_->Init(object_->transform_.translate,1.0f,onCollision,onCollisionMapChip);

	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Enemy","speed",speed_);
	variables->addValue("Game","Enemy","minJumpPower",minJumpPower_);
	variables->addValue("Game","Enemy","maxJumpPower",maxJumpPower_);
	variables->addValue("Game","Enemy","cloneOffset",cloneOffset_);
	variables->addValue("Game","Enemy","stealEnergy_",stealEnergy_);
	variables->addValue("Game","Enemy","numberOfClones2Create_",numberOfClones2Create_);
	variables->DestroyItem("Game","Enemy","numberOfClones2Create_");
}

void Enemy::Update(std::list<std::unique_ptr<Enemy>>& enemies)
{
	// 死んだら スキップ
	if(!isAlive_) { return; }
	if(object_->transform_.translate.y <= -6.0f)
	{
		isAlive_ = false;
		return;
	}

	isHurt_ = false;

	if(preOnGround_ && !isOnGround_)
	{// 地面を離れた
		if(onWavingMapChip_)
		{
			onWavingMapChip_ = false;
			velocity_.y = Lerp<float>(mapObjectWaveDistance_ / waveRange_,minJumpPower_,maxJumpPower_);
		}
	} else if(!preOnGround_ && isOnGround_)
	{// 着地した 瞬間
		// waveRange を 複製体作成 の 切符として 機能させる
		if (waveRange_ != 0.0f)
		{
			if (object_->transform_.translate.y <= 1)
			{
				stage_->ChargeEnergy(stolenEnergy_);

				isHurt_ = true;
				isClone_ = true;
				--numberOfClones2Create_;
				if (numberOfClones2Create_ < 0.0f)
				{
					isAlive_ = false;
					return;
				}
				enemies.emplace_back(CreateClone());
				waveRange_ = 0.0f;
				mapObjectWaveDistance_ = 0.0f;
			}
		}
	}

	const float& deltaTime = DeltaTime::getInstance()->getDeltaTime();
	// 移動
	// wave している 床の上にいるときは 動かない
	if(!onWavingMapChip_)
	{
		Float2 moveVal = moveDirection_ * (speed_ * deltaTime);
		velocity_.x = moveVal.x;
		velocity_.z = moveVal.y;
	} else
	{
		velocity_.x = 0.0f;
		velocity_.z = 0.0f;
	}

	if(isOnGround_)
	{
		if(!isClone_)
		{
			stolenEnergy_ += stage_->StealEnergy(stealEnergy_ * deltaTime);
		}
		// 範囲内でなければ
		if (waveRange_ == 0.0f)
		{
			velocity_.y = 0.0f;
		}else 
		{ // 範囲内なら
			velocity_.y = 0.7f;
		}
	} else
	{
		velocity_.y -= 9.8f * deltaTime;
	}

	object_->transform_.translate += velocity_;

	collider_->SetPosition(object_->transform_.translate);

	preOnGround_ = isOnGround_;
	isOnGround_ = false;

	object_->UpdateMatrix();
}

void Enemy::Draw()
{
	/*object_->UpdateMatrix();*/
	object_->Draw();
}

void Enemy::CloneInitialize(Float3 spawnPos,Float2 moveDirection,ModelManager::ModelData* modelData,int32_t numberOfClones2Create)
{
	isAlive_ = true;
	isClone_ = true;

	object_ = std::make_unique<Object3D>();
	object_->model_ = modelData;
	object_->transform_.translate = spawnPos;

	moveDirection_ = moveDirection;
	moveDirection_ = Float2::Normalize(moveDirection_);
	object_->transform_.rotate.y = atan2(moveDirection_.y,moveDirection_.x);

	collider_ = std::make_unique<Collider>();

	auto onCollision = []([[maybe_unused]] Collider* a) {};

	auto onCollisionMapChip = [this](MapChipField::MapObject* mapObject)
	{
		isOnGround_ = true;

		// Wave が 発動した 床
		if(mapObject->isWave && mapObject->waveDelay <= 0.0f)
		{
			// 原点からの方向
			Float2 moveDirectionXZ = Float2({
				static_cast<float>(mapObject->GetIndexSet().xIndex - mapObject->addressOfWaveOrigin_.xIndex),
				static_cast<float>(mapObject->addressOfWaveOrigin_.zIndex - mapObject->GetIndexSet().zIndex)
											});
			onWavingMapChip_ = true;

			waveRange_ = mapObject->waveRange_;

			mapObjectWaveDistance_ = Float2::Length(moveDirectionXZ);
			moveDirection_ = Float2::Normalize(moveDirectionXZ);
			object_->transform_.rotate.y = atan2(moveDirection_.y,moveDirection_.x);
		}
		object_->transform_.translate.y = mapObject->GetTranslate().y + collider_->GetRadius();
	};

	collider_->Init(object_->transform_.translate,1.0f,onCollision,onCollisionMapChip);

	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Enemy","speed",speed_);
	variables->addValue("Game","Enemy","minJumpPower",minJumpPower_);
	variables->addValue("Game","Enemy","maxJumpPower",maxJumpPower_);
}

// 分裂体 の 作成
Enemy* Enemy::CreateClone()
{
	Enemy* clone = new Enemy();
	// 横に出現させる
	Float3 cloneSpawnPos = TransformMatrix(cloneOffset_,object_->transform_.MakeAffineMatrix());
	// Clone 用の 初期化
	clone->CloneInitialize(cloneSpawnPos,moveDirection_,object_->model_,numberOfClones2Create_);
	clone->SetStage(stage_);

	// クローン先のパーティクル発生フラグを立てる
	clone->isCloneThisFrame_ = true;

	return clone;
}