#pragma once
#include "PlayerMoveParticle.h"
#include <list>

class PlayerMoveEmitter
{
public:
	PlayerMoveEmitter();
	~PlayerMoveEmitter();

	void Initialize(ModelManager::ModelData* model, uint32_t textureHandle);
	void Update();
	void Draw();
	void Emit(Float3 translation);

private:
	ModelManager::ModelData* model_;
	uint32_t textureHandle_;
	std::list<std::unique_ptr<PlayerMoveParticle>> particleList_;
	uint32_t timer_ = 0;
	uint32_t interval = 15;
};
