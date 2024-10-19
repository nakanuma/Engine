#pragma once
#include "EnemyLandingParticle.h"
#include <list>

class EnemyLandingEmitter {
public:
	EnemyLandingEmitter();
	~EnemyLandingEmitter();

	void Initialize(ModelManager::ModelData* model, uint32_t textureHandle);
	void Update();
	void Draw();
	void Emit(Float3 translation);

private:
	// モデル
	ModelManager::ModelData* model_;
	// モデルのテクスチャ
	uint32_t textureHandle_;
	// パーティクルリスト
	std::list<std::unique_ptr<EnemyLandingParticle>> particleList_;
	// 一度の発生パーティクル数
	const uint32_t kParticleNum = 4;
};
