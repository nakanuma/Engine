#include "Enemy.h"

#include "Application/myRandom/MyRandom.h"
#include "DeltaTime.h"
#include "GlobalVariables.h"

#include <cmath>

Enemy::~Enemy()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->DestroyItem("Game","Enemy","speed");
}

void Enemy::Initialize(ModelManager::ModelData* modelData)
{
	object_ = std::make_unique<Object3D>();
	object_->model_ = modelData;

	MyRandom randX = MyRandom(-1.0f,1.0f);
	MyRandom randZ = MyRandom(-1.0f,1.0f);

	moveDirection_ = {randX(),randZ()};
	moveDirection_ = Float2(moveDirection_);
	object_->transform_.rotate.y = atan2(moveDirection_.y,moveDirection_.x);

	collider_ = std::make_unique<Collider>();
	collider_->Init(object_->transform_.translate,1.0f,[]([[maybe_unused]] Collider* a) {});

	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Enemy","speed",speed_);
}

void Enemy::Update()
{
	if(preOnGround_ && !isOnGround_)
	{// 地面を離れた 
		velocity_.y = floorVelocityY_;
	} else if(!preOnGround_ && isOnGround_)
	{// 着地した 瞬間
		floorVelocityY_ = 0;
	}

	const float& deltaTime = DeltaTime::getInstance()->getDeltaTime();
	// 移動
	Float2 moveVal = moveDirection_ * (speed_ * deltaTime);
	velocity_.x = moveVal.x;
	velocity_.z = moveVal.y;
	if(isOnGround_)
	{
		velocity_.y = 9.8f * deltaTime;
	} else
	{
		velocity_.y -= 9.8f * deltaTime;
	}


	object_->transform_.translate += velocity_;

	object_->UpdateMatrix();

	collider_->SetPosition(object_->transform_.translate);

	preOnGround_ = isOnGround_;
	isOnGround_ = false;
}

void Enemy::Draw()
{
	object_->Draw();
}

void Enemy::OnCollisionMapChip(const Float2& dir,float chipY,float mapChipVelocityY)
{
	isOnGround_ = true;
	floorVelocityY_ += mapChipVelocityY;
	object_->transform_.translate.y = chipY;
}