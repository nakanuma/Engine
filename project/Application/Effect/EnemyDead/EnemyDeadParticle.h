#pragma once
#include "ModelManager.h"
#include "Object3D.h"

class EnemyDeadParticle {
public:
	EnemyDeadParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity, int32_t delTime);
	~EnemyDeadParticle();

	void Update();
	void Draw();

	bool GetDelFlag() { return false; }

private:
	ModelManager::ModelData* model_;
	Object3D object_;
	Float3 velocity_;
	bool del_ = false;

	int32_t timer_;
	int32_t delTime_;
};
