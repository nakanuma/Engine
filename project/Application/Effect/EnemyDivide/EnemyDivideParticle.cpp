#include "EnemyDivideParticle.h"
#include <cassert>
#include "Easing.h"

EnemyDivideParticle::EnemyDivideParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity)
{
	// 引数で受け取った値を設定
	assert(model);
	model_ = model;
	object_.transform_.translate = translation;
	const float kScale = 0.5f;
	object_.transform_.scale = { kScale, kScale, kScale };
	object_.model_ = model_;
	object_.model_->material.textureHandle = textureHandle;
	velocity_ = velocity;

	// 経過時間の設定
	timer_ = 0;
}

EnemyDivideParticle::~EnemyDivideParticle()
{
}

void EnemyDivideParticle::Update()
{
	if (timer_++ >= kLifeTime) {
		del_ = true;
	}

	// 移動
	object_.transform_.translate += velocity_ * speed_;

	if (speed_ >= 0.0f) {
		speed_ -= 0.015f;
	} else {
		speed_ = 0.0f;
	}

	float t = static_cast<float>(timer_) / kLifeTime;
	float scale = Easing::Lerp(0.5f, 0.0f, Easing::EaseInQuint(t));
	object_.transform_.scale = { scale, scale, scale };

	// オブジェクトの行列更新
	object_.UpdateMatrix();
}

void EnemyDivideParticle::Draw()
{
	// オブジェクトの描画
	object_.Draw();
}
