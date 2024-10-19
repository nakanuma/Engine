#pragma once
#include "ModelManager.h"
#include "Object3D.h"

class PlayerMoveParticle
{
public:
	PlayerMoveParticle(ModelManager::ModelData* model, uint32_t textureHandle, Float3 translation, Float3 velocity);
	~PlayerMoveParticle();

	void Update();
	void Draw();

	bool GetDelFlag() { return del_; }

private:
	ModelManager::ModelData* model_ = nullptr;
	Object3D object_;
	Float3 velocity_;
	bool del_ = false;
	int32_t timer_;
	const int32_t kTotalTime = 60;
};

