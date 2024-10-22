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

	// 速度の設定
	const float vel = 0.1f;
	std::uniform_real_distribution<float> distVel(-vel, vel);

	// 消えるまでの時間をランダムに生成
	const uint32_t minDelTime = 60;
	const uint32_t maxDelTime = 120;
	std::uniform_int_distribution<uint32_t> distDelTime(minDelTime, maxDelTime);

	int32_t delTime = distDelTime(rng);

	for (uint32_t i = 0; i < kParticleNum; i++) {
		// 速度の生成
		Float3 velocity = {distVel(rng), distVel(rng), distVel(rng)};

		std::unique_ptr<EnemyDeadParticle> newParticle = 
			std::make_unique<EnemyDeadParticle>(model_, textureHandle_, translation, velocity, delTime);
		particleList_.push_back(std::move(newParticle));
	}
}
