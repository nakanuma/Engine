#pragma once
#include "ModelManager.h"
#include "Object3D.h"

class PlayerAttackParticle {
public:
	PlayerAttackParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity);
	~PlayerAttackParticle();

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
	float acceleratorY = 0.01f;
	// 削除フラグ
	bool del_ = false;
	// 経過時間を計測
	int32_t timer_;
};
