#pragma once
#include "PlayerAttackParticle.h"
#include <list>

class PlayerAttackEmitter {
public:
	PlayerAttackEmitter();
	~PlayerAttackEmitter();

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
	std::list<std::unique_ptr<PlayerAttackParticle>> particleList_;
	// 一度の発生パーティクル数
	const uint32_t kParticleNum = 5;
};
