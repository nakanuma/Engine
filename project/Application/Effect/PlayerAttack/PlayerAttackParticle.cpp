#include "PlayerAttackParticle.h"
#include <cassert>

PlayerAttackParticle::PlayerAttackParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity) {
	// 引数で受け取った値を設定
	assert(model);
	model_ = model;
	object_.transform_.translate = translation;
	object_.transform_.scale = {0.5f, 0.5f, 0.5f};
	object_.model_ = model_;
	object_.model_->material.textureHandle = textureHandle;
	velocity_ = velocity;

	// 経過時間の設定
	timer_ = 0;
}

PlayerAttackParticle::~PlayerAttackParticle() {}

void PlayerAttackParticle::Update() {
	// タイマー増加
	timer_++;

	// 指定フレーム経過したら落下させる
	if (timer_ >= 30) {
		velocity_.y -= acceleratorY;
	}

	// Y座標が指定以下になったら削除
	if (object_.transform_.translate.y <= 0.0f) {
		del_ = true;
	}

	// 移動
	object_.transform_.translate += velocity_;

	// オブジェクトの行列更新
	object_.UpdateMatrix();
}

void PlayerAttackParticle::Draw() {
	// オブジェクトの描画
	object_.Draw();
}
