#include "EnemyDeadParticle.h"
#include <cassert>

EnemyDeadParticle::EnemyDeadParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity, int32_t delTime) 
{ 
	assert(model);
	model_ = model;
	object_.transform_.translate = translation;
	const float kScale = 1.0f;
	object_.transform_.scale = {kScale, kScale, kScale};
	object_.model_ = model;
	object_.model_->material.textureHandle = textureHandle;
	velocity_ = velocity;

	timer_ = 0;
	delTime_ = delTime;
}

EnemyDeadParticle::~EnemyDeadParticle() {}

void EnemyDeadParticle::Update() { 
	timer_++; 

	// 削除時間に到達したら削除
	if (timer_ > delTime_) {
		del_ = true;
	}

	// 移動
	object_.transform_.translate += velocity_;

	object_.UpdateMatrix();
}

void EnemyDeadParticle::Draw() { 
	object_.Draw(); 
}
