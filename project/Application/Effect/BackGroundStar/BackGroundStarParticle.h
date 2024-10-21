#pragma once
#include "MyMath.h"
#include "Sprite.h"
#include "SpriteCommon.h"

class BackGroundStarParticle {
public:
	BackGroundStarParticle(Float2 position, Float2 size, uint32_t textureHandle, SpriteCommon* spriteCommon);
	~BackGroundStarParticle();

	void Update();
	void Draw();
	
	bool GetDelFlag() { return del_; }

private:
	Sprite sprite_;
	bool del_ = false;

	float alpha_;
	bool IsIncreasing_; // trueならalpha増加中、falseならalpha減少中
	const float increasingSpeed_ = 0.02f;
};
