#pragma once
#include "ModelManager.h"
#include "Object3D.h"

class EnemyDeadParticle {
public:
	EnemyDeadParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 rotate, float scale, Float3 velocity, int32_t delTime);
	~EnemyDeadParticle();

	void Update();
	void Draw();

	bool GetDelFlag() { return del_; }

private:
	ModelManager::ModelData* model_;
	Object3D object_;
	Float3 velocity_;
	float scale_;
	bool del_ = false;
	float acceleratorY = 0.001f;

	int32_t timer_;
	int32_t delTime_;
};
