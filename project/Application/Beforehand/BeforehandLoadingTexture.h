#pragma once
#include "TextureManager.h"

#include <deque>

class BeforehandLoadingTexture
{
public:
	// Textureの読み込み
	void Initialize();

private:
	// チュートリアルで使用するテクスチャ
	std::deque<uint32_t> tutorialTextTextures_;
	std::deque<uint32_t> tutorialTaskGuidTextures_;
};

