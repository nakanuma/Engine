#pragma once
#include "BaseScene.h"
#include "Camera.h"
#include "Input.h"
#include "LightManager.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "SoundManager.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"

#include "Application/Stage/Stage.h"
#include "Application/UI/UI.h"

// ゲームプレイシーン
class TitleScene : public BaseScene
{
public:
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;

private:
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	LightManager* lightManager = nullptr;
	Input* input = nullptr;

	///
	/// ↓ ゲームシーン用
	///
	Stage* stage_;

	uint32_t buttonTextureIndex_;
	std::unique_ptr<UI> buttonUI_;
	Float2 buttonUiOffset_;
	float t_;
	float signT_;
};