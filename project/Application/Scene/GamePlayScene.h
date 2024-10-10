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

#include <list>

#include "Application/Player/Player.h"
#include "Application/MapChip.h"
#include "Application/Collider/CollisionManager.h"
#include "Application/Enemy/Enemy.h"

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

	/// 衝突判定と応答
	void CheckAllCollisions();

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

	ModelManager::ModelData enemyModel;
	std::list<std::unique_ptr<Enemy>> enemies_;

	std::unique_ptr<Player> player_;

	// 衝突マネージャ
	std::unique_ptr<CollisionManager> collisionManager_;
};

