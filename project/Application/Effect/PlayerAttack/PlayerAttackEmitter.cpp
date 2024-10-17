#include "PlayerAttackEmitter.h"
#include <cassert>
#include <random>

PlayerAttackEmitter::PlayerAttackEmitter() {}

PlayerAttackEmitter::~PlayerAttackEmitter() {}

void PlayerAttackEmitter::Initialize(ModelManager::ModelData* model, uint32_t textureHandle) { 
	assert(model); 
	model_ = model;
	textureHandle_ = textureHandle;
}

void PlayerAttackEmitter::Update() {
	// リスト内のパーティクル全てを更新
	for (auto itr = particleList_.begin(); itr != particleList_.end();) {
		(*itr)->Update();

		// DelFlagがtrueだったらリストから削除
		if ((*itr)->GetDelFlag()) {
			itr = particleList_.erase(itr);
			// eraseしなかった場合イテレーターをインクリメント
		} else {
			itr++;
		}
	}
}

void PlayerAttackEmitter::Draw() {
	// リスト内のパーティクル全てを描画
	for (auto& particle : particleList_) {
		particle->Draw();
	}
}

void PlayerAttackEmitter::Emit(Float3 translation) {
	// 乱数生成器を初期化
	std::random_device rd;
	std::mt19937 rng(rd());

	// 各速度を決める
	const float kMaxVelX = 0.12f;
	const float kMaxVelY = 0.20f;
	const float kMaxVelZ = 0.10f;
	std::uniform_real_distribution<float> distX(-kMaxVelX, kMaxVelX);
	std::uniform_real_distribution<float> distY(0.0f, kMaxVelY); // 最初に下向きにならないように設定
	std::uniform_real_distribution<float> distZ(-kMaxVelZ, kMaxVelZ);

	for (uint32_t i = 0; i < kParticleNum; i++) {
		// 速度ベクトルをランダムに生成
		Float3 velocity(distX(rng), distY(rng), distZ(rng));

		std::unique_ptr<PlayerAttackParticle> newParticle = 
			std::make_unique<PlayerAttackParticle>(model_, textureHandle_, translation, velocity);
		particleList_.push_back(std::move(newParticle));
	}
}
