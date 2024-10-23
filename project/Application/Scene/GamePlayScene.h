#pragma once
#include "BaseScene.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "Input.h"
#include "LightManager.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "SoundManager.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "SRVManager.h"
#include "TextureManager.h"

#include <list>
#include <memory>

#include "Application/Stage/Stage.h"
#include "Application/UI/UI.h"

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
	void ButtonSpriteUI(Sprite* sprite, bool isActive, const Float2& position, const Float2& size);
private:
	Camera* camera = nullptr;
	std::unique_ptr<DebugCamera>   debugCamera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	std::unique_ptr<SRVManager> srvManager = nullptr;
	std::unique_ptr<ParticleManager> particleManager = nullptr;
	Input* input = nullptr;
	LightManager* lightManager = nullptr;

	Stage* stage_;

	///
	/// ↓ ゲームシーン用
	///
	
	std::function<void()> currentUpdate;
	void EnterSceneUpdate();
	void SceneUpdate();

	float leftTime_;
	float enterSceneMaxTime_;

	std::unique_ptr<Sprite> cutInTexture_;
	Float2 startCutInTexturePos_;
	Float2 stopCutInTexturePos_;
	Float2 endCutInTexturePos_;
	bool isCutIn_;

	ModelManager::ModelData planeModel;

	std::array<std::unique_ptr<UI>, 5> buttonSprite_;

	std::function<void(Sprite*)> buttonUpdate_[5];

	// テスト用サウンドデータ
	SoundManager::SoundData soundData1;
};