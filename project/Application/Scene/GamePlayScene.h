#pragma once
#include "BaseScene.h"
#include "Camera.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "SoundManager.h"
#include "Input.h"
#include "Application/MapChip.h"
#include "LightManager.h"

#include "Application/Player/Player.h"

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

	void GenerateBloks();

private:
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	Input* input = nullptr;
	LightManager* lightManager = nullptr;

	///
	/// ↓ ゲームシーン用
	///

	// モデルデータ
	ModelManager::ModelData model_;
	// 3Dオブジェクト
	std::unique_ptr<Object3D> object_;

	/// マップチップ
	std::unique_ptr<MapChipField> mapChip_;
	
	// モデルデータ(マップチップ)
	ModelManager::ModelData modelBlock_;
	// 複数並べるために配列にする(マップチップ)
	std::vector<std::vector<std::unique_ptr<Object3D>>> objectBlocks_;


	std::unique_ptr<Player> player_;
};

