#include "PlayerMoveParticle.h"
#include <cassert>
#include "Easing.h"

PlayerMoveParticle::PlayerMoveParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity)
{
	assert(model);
	model_ = model;
	object_.transform_.translate = translation;
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

	// 経過時間に応じてスケールを変更
	float t = static_cast<float>(timer_) / kTotalTime;
	t = (t > 1.0f) ? 1.0f : t;

	float scale = Easing::Lerp(0.2f, 0.0f, Easing::EaseInQuart(t));
	object_.transform_.scale = { scale, scale, scale };

	// 更新
	object_.UpdateMatrix();
}

void PlayerMoveParticle::Draw()
{
	object_.Draw();
}
