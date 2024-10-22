#pragma once
#include "EnemyDeadParticle.h"
#include <list>

class EnemyDeadEmitter {
public:
	EnemyDeadEmitter();
	~EnemyDeadEmitter();

	void Initialize(ModelManager::ModelData* model, uint32_t textureHandle);
	void Update();
	void Draw();
	void Emit(Float3 translation);

private:
	ModelManager::ModelData* model_;
	uint32_t textureHandle_;
	std::list<std::unique_ptr<EnemyDeadParticle>> particleList_;
	const uint32_t kParticleNum = 8;
};
