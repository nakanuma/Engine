#include "BeforehandLoadingTexture.h"

void BeforehandLoadingTexture::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// チュートリアルで使用するテクスチャ

	std::string directory = "./resources/Images/tutorial/";
	for (int32_t i = 1; i <= 4; i++)
	{
		tutorialTextTextures_.push_back(TextureManager::Load(directory + "tutorial_" + std::to_string(i) + ".png", dxBase->GetDevice()));
	}
	tutorialTextTextures_.push_back(TextureManager::Load(directory + "tutorial_" + std::to_string(7) + ".png", dxBase->GetDevice()));

	for (int32_t i = 9; i <= 10; i++)
	{
		tutorialTextTextures_.push_back(TextureManager::Load(directory + "tutorial_" + std::to_string(i) + ".png", dxBase->GetDevice()));
	}
	for (int32_t i = 12; i <= 13; i++)
	{
		tutorialTextTextures_.push_back(TextureManager::Load(directory + "tutorial_" + std::to_string(i) + ".png", dxBase->GetDevice()));
	}

	for (size_t i = 0; i < 6; i++)
	{
		tutorialTaskGuidTextures_.push_back(TextureManager::Load(directory + "tutorial_5_" + std::to_string(i) + ".png", dxBase->GetDevice()));
	}

	tutorialTaskGuidTextures_.push_back(TextureManager::Load(directory + "tutorial_8" + ".png", dxBase->GetDevice()));

	for (size_t i = 0; i < 6; i++)
	{
		tutorialTaskGuidTextures_.push_back(TextureManager::Load(directory + "tutorial_11_" + std::to_string(i) + ".png", dxBase->GetDevice()));
	}
}
