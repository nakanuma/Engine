#include "EnemyDeadEmitter.h"
#include <cassert>
#include <random>

EnemyDeadEmitter::EnemyDeadEmitter() {}

EnemyDeadEmitter::~EnemyDeadEmitter() {}

void EnemyDeadEmitter::Initialize(ModelManager::ModelData* model, uint32_t textureHandle) { 
	assert(model); 
	model_ = model;
	textureHandle_ = textureHandle;
}

void EnemyDeadEmitter::Update() {
	for (auto itr = particleList_.begin(); itr != particleList_.end();) {
		(*itr)->Update();

		if ((*itr)->GetDelFlag()) {
			itr = particleList_.erase(itr);
		} else {
			itr++;
		}
	}
}

void EnemyDeadEmitter::Draw() {
	for (auto& particle : particleList_) {
		particle->Draw();
	}
}

void EnemyDeadEmitter::Emit(Float3 translation) {
	// 乱数生成器を初期化
	std::random_device rd;
	std::mt19937 rng(rd());

	// 速度の設定（XとZが共通）
	const float vel = 0.05f;
	std::uniform_real_distribution<float> distVelX(-vel, vel);
	std::uniform_real_distribution<float> distVelZ(-vel, vel);

	const float velY = 0.005f;
	std::uniform_real_distribution<float> distVelY(0.0f, velY);

	// スケールの設定
	const float minScale = 0.3f;
	const float maxScale = 0.6f;
	std::uniform_real_distribution<float> distScale(minScale, maxScale);

	// 消えるまでの時間をランダムに生成
	const uint32_t minDelTime = 60;
	const uint32_t maxDelTime = 120;
	std::uniform_int_distribution<uint32_t> distDelTime(minDelTime, maxDelTime);

	// 生成位置のオフセットを設定
	const int32_t minOffset = static_cast<int32_t>(-0.5f);
	const int32_t maxOffset = static_cast<int32_t>(0.5f);
	std::uniform_int_distribution<int32_t> distOffset(minOffset, maxOffset);

	for (uint32_t i = 0; i < kParticleNum; i++) {
		// 速度の生成
		Float3 velocity = {distVelX(rng), distVelY(rng), distVelZ(rng)};
		// スケールの生成
		float scale = distScale(rng);
		// 削除までの時間生成
		int32_t delTime = distDelTime(rng);
		// オフセット生成
		int32_t offset = distOffset(rng);

		Float3 rTranslation = {translation.x + offset, translation.y + offset + 2.0f, translation.z + offset};

		std::unique_ptr<EnemyDeadParticle> newParticle = 
			std::make_unique<EnemyDeadParticle>(model_, textureHandle_, rTranslation, scale, velocity, delTime);
		particleList_.push_back(std::move(newParticle));
	}
}
