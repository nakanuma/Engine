#pragma once
#include "EnemyDivideParticle.h"
#include <list>

class EnemyDivideEmitter
{
public:
	EnemyDivideEmitter();
	~EnemyDivideEmitter();

	void Initialize(ModelManager::ModelData* model, uint32_t textureHandle);
	void Update();
	void Draw();
	void Emit(Float3 translation);

private:
	ModelManager::ModelData* model_;
	uint32_t textureHandle_;
	std::list<std::unique_ptr<EnemyDivideParticle>> particleList_;
	const uint32_t kParticleNum = 4;
};

