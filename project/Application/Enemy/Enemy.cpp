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

	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Enemy","speed",speed_);
}

void Enemy::Update()
{
	Float2 moveVal = moveDirection_ * (speed_ * DeltaTime::getInstance()->getDeltaTime());
	object_->transform_.translate = {object_->transform_.translate.x + moveVal.x,object_->transform_.translate.y,object_->transform_.translate.z + moveVal.y};
	object_->UpdateMatrix();
}

void Enemy::Draw()
{
	object_->Draw();
}
