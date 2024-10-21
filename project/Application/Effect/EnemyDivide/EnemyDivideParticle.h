#pragma once
#include "ModelManager.h"
#include "Object3D.h"

class EnemyDivideParticle
{
public:
	EnemyDivideParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity);
	~EnemyDivideParticle();

	void Update();
	void Draw();

	bool GetDelFlag() { return del_; }

private:
	ModelManager::ModelData* model_ = nullptr;
	Object3D object_;
	Float3 velocity_;
	float speed_ = 0.3f;
	bool del_ = false;
	int32_t timer_;
	const int32_t kLifeTime = 30;
};

