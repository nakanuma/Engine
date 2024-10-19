#include "PlayerMoveParticle.h"
#include <cassert>

PlayerMoveParticle::PlayerMoveParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity)
{
	assert(model);
	model_ = model;
	object_.transform_.translate = translation;
	const float kScale = 0.2f;
	object_.transform_.scale = { kScale, kScale, kScale };
	object_.model_ = model_;
	object_.model_->material.textureHandle = textureHandle;
	velocity_ = velocity;

	timer_ = 0;
}

PlayerMoveParticle::~PlayerMoveParticle()
{

}

void PlayerMoveParticle::Update()
{
	// タイマー増加
	timer_++;

	// 合計時間を過ぎたら削除
	if (timer_ >= kTotalTime) {
		del_ = true;
	}

	// 移動
	object_.transform_.translate += velocity_;

	object_.UpdateMatrix();
}

void PlayerMoveParticle::Draw()
{
	object_.Draw();
}
