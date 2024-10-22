#pragma once
#include "ModelManager.h"
#include "Object3D.h"

class EnemyLandingParticle {
public:
	EnemyLandingParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity, Float3 rotateSpeed);
	~EnemyLandingParticle();

	void Update();
	void Draw();

	bool GetDelFlag() { return del_; }

private:
	// モデル
	ModelManager::ModelData* model_ = nullptr;
	// オブジェクト
	Object3D object_;
	// 速度
	Float3 velocity_;
	// 回転速度
	Float3 rotateSpeed_;
	float acceleratorY = 0.008f;
	const float kSpeed_ = 0.07f;
	// 削除フラグ
	bool del_ = false;
	// 経過時間を計測
	int32_t timer_;
};
