#pragma once

#include <functional>
#include <memory>

#include "Sprite.h"
#include "SpriteCommon.h"

#include <string>

#include "Float2.h"
#include "Float3.h"
#include "Float4.h"
#include <stdint.h>

class UI{
public:
	void Init(const std::string& scene,
			  const std::string& uiName,
			  std::uint32_t textureIndex,
			  SpriteCommon* spriteCommon);
	void Update();

	void Draw();
private:
	Float2 pos_;
	Float2 size_ = {10.0f,10.0f};
	Float4 color_ = {1.0f,1.0f,1.0f,1.0f};
	Float2 anchor_ = {0.0f,0.0f};
	bool isFlipX_ = false;
	bool isFlipY_ = false;


	std::unique_ptr<Sprite> sprite_;

	std::function<void(Sprite*)> update_;
public:
	Float2 GetPosition()const{return pos_;}
	const Sprite* getSprite()const{ return sprite_.get(); }
	void setSprite(Sprite* sprite){ sprite_.reset(sprite); }

	void setUpdate(std::function<void(Sprite*)> update){ update_ = update; }
};