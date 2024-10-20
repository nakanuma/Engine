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

#include <queue>

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

	Float3 cameraPosWhenEnterScene_;
	Float3 cameraHomePos_;

	std::queue<std::function<void()>> tutorialTextUpdate_;
	std::queue<std::function<bool()>> tutorialTask_;
	bool isPrePlayerAttack_;
	std::unordered_map<Enemy*,bool> wasAttackedFormPlayer_;
};