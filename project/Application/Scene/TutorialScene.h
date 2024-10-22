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

#include <deque>
#include <list>

#include "Application/Stage/Stage.h"
#include "Application/UI/UI.h"

// ゲームプレイシーン
class TutorialScene
	: public BaseScene
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
	void TextTextureUpdate(uint32_t textureSum);
private:
	Camera* camera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	LightManager* lightManager = nullptr;
	Input* input = nullptr;

	Stage* stage_;

	///
	/// ↓ タイトルシーン用
	///

	std::function<void()> currentUpdate_;

	void EnterSceneUpdate();
	void SceneUpdate();
	void OutSceneUpdate();
	float t_;
	float leftTime_;
	float enterSceneMaxTime_;
	float outSceneMaxTime_;

	bool doTask_;
	std::deque<uint32_t> tutorialTextTextures_;
	std::deque<uint32_t> tutorialTaskGuidTextures_;
	uint32_t currentTextTextureIndex_;

	std::unique_ptr<Sprite> currentTextTexture_;
	Float2 textTexturePos_;
	std::unique_ptr<Sprite> currentTaskGuidTexture_;
	Float2 taskGuidTexturePos_;

	std::deque<std::function<void()>> tutorialTextUpdate_;
	std::deque<std::function<bool()>> tutorialTask_;

	// タスクに使う 変数 群
	int32_t playerAttackNum_;
	int32_t enemyHurtNum_;
};