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

	uint32_t buttonTextureIndex_;
	std::unique_ptr<UI> buttonUI_;
	std::function<void(Sprite*)> buttonUpdateWhenEnterScene_;
	std::function<void(Sprite*)> buttonUpdateWhenSceneUpdate_;
	std::function<void(Sprite*)> buttonUpdateWhenOutScene_;

	std::array<std::unique_ptr<UI>,2> cloudSprite_;

	ModelManager::ModelData titleTextModel_;
	std::unique_ptr<Object3D> titleTextObject_;

	Float2 buttonUiOffset_;
	float signT_;
};