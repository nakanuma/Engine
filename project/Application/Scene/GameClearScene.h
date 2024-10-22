#pragma once
#include "BaseScene.h"

#include <functional>

#include "Camera.h"
#include "DebugCamera.h"
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

#include "Application/TexturePlane/TexturePlane.h"

// ゲームプレイシーン
class GameClearScene
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
	std::unique_ptr<DebugCamera> debugCamera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	Input* input = nullptr;
	LightManager* lightManager = nullptr;

	Stage* stage_;

	///
	/// ↓ ゲームクリア用
	///

	std::function<void()> currentUpdate_;
	
	void EnterSceneUpdate();
	void SceneUpdate();
	void OutSceneUpdate();

	float leftTime_;

	float enterSceneMaxTime_;
	float outSceneMaxTime_;

	// Camera
	Float3 cameraPosWhenOutScene_;
	Float3 cameraPosWhenEnterScene_;

	// clearText
	uint32_t clearTextTextureIndex_;
	std::unique_ptr<TexturePlane> clearTextPlane_;
public:
};