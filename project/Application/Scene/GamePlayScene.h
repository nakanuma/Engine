#pragma once
#include "BaseScene.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "SoundManager.h"
#include "Input.h"
#include "LightManager.h"
#include "SRVManager.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"

#include <list>

#include "Application/Stage/Stage.h"

// ゲームプレイシーン
class GamePlayScene : public BaseScene
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
#ifdef _DEBUG // デバッグカメラ用
	bool useDebugCamera = false;    // デバッグカメラが有効か
	Transform savedCameraTransform; // 通常カメラのTransformを保持

	void DebugCameraUpdate(Input* input);
#endif

private:
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<DebugCamera> debugCamera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	std::unique_ptr<SRVManager> srvManager = nullptr;
	std::unique_ptr<ParticleManager> particleManager = nullptr;
	Input* input = nullptr;
	LightManager* lightManager = nullptr;

	///
	/// ↓ ゲームシーン用
	///
	std::unique_ptr<Stage> stage_;

	ModelManager::ModelData planeModel;
};