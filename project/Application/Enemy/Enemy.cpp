#include "Enemy.h"

#include "Application/Collision/Collider.h"
#include "Application/myRandom/MyRandom.h"
#include "DeltaTime.h"
#include "GlobalVariables.h"
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

	object_ = std::make_unique<Object3D>();
	object_->model_ = modelData;
	object_->transform_.translate = spawnPos;

	moveDirection_ = moveDirection;
	moveDirection_ = Float2::Normalize(moveDirection_);
	object_->transform_.rotate.y = atan2(moveDirection_.y,moveDirection_.x);

	collider_ = std::make_unique<Collider>();

	auto onCollision = []([[maybe_unused]] Collider* a) {};

	auto onCollisionMapChip = [this](MapChipField::MapObject *mapObject){
		isOnGround_ = true;

		// Wave が 発動した 床
		if(mapObject->isWeve && mapObject->waveDelay <= 0.0f)
		{
			// 原点からの方向
			Float2 moveDirectionXZ = Float2({
				static_cast<float>(mapObject->GetIndexSet().xIndex - mapObject->addressOfWaveOrigin_.xIndex ),
				static_cast<float>(mapObject->addressOfWaveOrigin_.zIndex - mapObject->GetIndexSet().zIndex)
													   });
			onWavingMapChip_ = true;

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

void Enemy::Update()
{
	if(!isAlive_)
	{
		return;
	}
	if(object_->transform_.translate.y <= -6.0f)
	{
		isAlive_ = false;
		return;
	}

	if(preOnGround_ && !isOnGround_)
	{// 地面を離れた
		// TODO
		// GlobalVariables の 脱却
			velocity_.y = Lerp<float>(1.0f - (mapObjectWaveDistance_ / waveRange),minJumpPower_,maxJumpPower_);
			onWavingMapChip_ = false;
	} else if(!preOnGround_ && isOnGround_)
	{// 着地した 瞬間
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
		velocity_;
	}
	
	if(isOnGround_)
	{
		velocity_.y = 0.0f;
	} else
	{
		velocity_.y -= 9.8f * deltaTime;
	}

	object_->transform_.translate += velocity_;

	collider_->SetPosition(object_->transform_.translate);

	preOnGround_ = isOnGround_;
	isOnGround_ = false;
}

void Enemy::Draw()
{
	object_->UpdateMatrix();
	object_->Draw();
}
