#include "BackGroundStarParticle.h"
#include <cassert>

BackGroundStarParticle::BackGroundStarParticle(Float2 position, Float2 size, uint32_t textureHandle, SpriteCommon* spriteCommon) { 
	sprite_.Initialize(spriteCommon, textureHandle);
	sprite_.SetPosition(position); 
	sprite_.SetSize(size);

	alpha_ = 0.0f;
	IsIncreasing_ = true;
}

BackGroundStarParticle::~BackGroundStarParticle() {}

void BackGroundStarParticle::Update() { 
	// alpha値の増加->減少->削除を行う
	if (IsIncreasing_) {
		alpha_ += increasingSpeed_;
		if (alpha_ >= 1.0f) {
			alpha_ = 1.0f;
			IsIncreasing_ = false; // 減少状態へ移行
		}
	} else {
		alpha_ -= increasingSpeed_;
		if (alpha_ <= 0.0f) {
			alpha_ = 0.0f;
			del_ = true;
		}
	}

	sprite_.SetColor({1.0f, 1.0f, 1.0f, alpha_});

	sprite_.Update();
}

void BackGroundStarParticle::Draw() { 
	sprite_.Draw(); 
}
