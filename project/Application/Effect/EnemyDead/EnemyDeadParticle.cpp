#include "EnemyDeadParticle.h"
#include <cassert>
#include "Easing.h"

EnemyDeadParticle::EnemyDeadParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, float scale, Float3 velocity, int32_t delTime) { 
	assert(model);
	model_ = model;
	object_.transform_.translate = translation;
	object_.model_ = model;
	object_.model_->material.textureHandle = textureHandle;
	velocity_ = velocity;
	scale_ = scale;

	timer_ = 0;
	delTime_ = delTime;
}

EnemyDeadParticle::~EnemyDeadParticle() {}

void EnemyDeadParticle::Update() { 
	timer_++; 

	// 時間に応じてスケールの縮小
	float t = static_cast<float>(timer_) / delTime_;
	float scale = Easing::Lerp(scale_, 0.0f, Easing::EaseInQuint(t));
	object_.transform_.scale = {scale, scale, scale};

	// 削除時間に到達したら削除
	if (timer_ > delTime_) {
		del_ = true;
	}

	// 上に移動するように
	if (timer_ > 10) {
		velocity_.y += acceleratorY;
	}
	acceleratorY += 0.00001f;

	// 移動
	object_.transform_.translate += velocity_;

	object_.UpdateMatrix();
}

void EnemyDeadParticle::Draw() { 
	object_.Draw(); 
}
