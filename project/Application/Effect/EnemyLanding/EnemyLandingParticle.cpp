#include "EnemyLandingParticle.h"
#include <cassert>

EnemyLandingParticle::EnemyLandingParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity, Float3 rotateSpeed) {
	// 引数で受け取った値を設定
	assert(model);
	model_ = model;
	object_.transform_.translate = translation;
	const float kScale = 0.25f;
	object_.transform_.scale = { kScale, kScale, kScale };
	object_.model_ = model_;
	object_.model_->material.textureHandle = textureHandle;
	velocity_ = velocity;
	rotateSpeed_ = rotateSpeed;

	// 経過時間の設定
	timer_ = 0;
}

EnemyLandingParticle::~EnemyLandingParticle() {}

void EnemyLandingParticle::Update() {
	// タイマー増加
	timer_++;

	// 指定フレーム経過したら落下させる
	if (timer_ >= 20) {
		velocity_.y -= acceleratorY;
	}

	// Y座標が指定以下になったら削除
	if (object_.transform_.translate.y <= 0.0f) {
		del_ = true;
	}

	// 移動
	object_.transform_.translate += velocity_;
	// 回転
	object_.transform_.rotate.x += rotateSpeed_.x;
	object_.transform_.rotate.y += rotateSpeed_.y;
	object_.transform_.rotate.z += rotateSpeed_.z;

	// オブジェクトの行列更新
	object_.UpdateMatrix();
}

void EnemyLandingParticle::Draw() {
	// オブジェクトの描画
	object_.Draw();
}
