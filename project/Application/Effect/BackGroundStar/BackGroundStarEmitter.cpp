#include "BackGroundStarEmitter.h"
#include <random>
#include <cassert>

BackGroundStarEmitter::BackGroundStarEmitter() {}

BackGroundStarEmitter::~BackGroundStarEmitter() {}

void BackGroundStarEmitter::Initialize(uint32_t textureHandle, SpriteCommon* spriteCommon) { 
	spriteCommon_ = spriteCommon;
	textureHandle_ = textureHandle; 
}

void BackGroundStarEmitter::Update() {
	for (auto itr = particleList_.begin(); itr != particleList_.end();) {
		(*itr)->Update();

		if ((*itr)->GetDelFlag()) {
			itr = particleList_.erase(itr);
		} else {
			itr++;
		}
	}
}

void BackGroundStarEmitter::Draw() {
	for (auto& particle : particleList_) {
		particle->Draw();
	}
}

void BackGroundStarEmitter::Emit() {
	// 乱数生成器を初期化
	std::random_device rd;
	std::mt19937 rng(rd());

	for (uint32_t i = 0; i < particleNum; i++) {
		// 生成位置をランダムに生成
		std::uniform_int_distribution<int32_t> distPosX(0, Window::GetWidth());
		std::uniform_int_distribution<int32_t> distPosY(0, Window::GetHeight());

		Float2 position = {static_cast<float>(distPosX(rng)), static_cast<float>(distPosY(rng))};

		// サイズをランダムに生成
		const float kMaxSize = 32.0f;
		const float kMinSize = 16.0f;

		std::uniform_real_distribution<float> distSize(kMinSize, kMaxSize);

		float size = distSize(rng);

		std::unique_ptr<BackGroundStarParticle> newParticle = std::make_unique<BackGroundStarParticle>(position, Float2{size, size}, textureHandle_, spriteCommon_);
		particleList_.push_back(std::move(newParticle));
	}
}
