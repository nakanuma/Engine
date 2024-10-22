#pragma once
#include "BackGroundStarParticle.h"
#include <list>

class BackGroundStarEmitter {
public:
	BackGroundStarEmitter();
	~BackGroundStarEmitter();

	void Initialize(uint32_t textureHandle, SpriteCommon* spriteCommon);
	void Update();
	void Draw();
	void Emit();

private:
	uint32_t textureHandle_;
	std::list<std::unique_ptr<BackGroundStarParticle>> particleList_;

	uint32_t particleNum = 5;

	SpriteCommon* spriteCommon_;

	int32_t timer_;
	const int32_t interval = 30;
};
