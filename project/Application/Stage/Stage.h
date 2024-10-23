#pragma once

#include <array>
#include <list>
#include <memory>

#include "ModelManager.h"
#include "SpriteCommon.h"
#include "Sprite.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Application/Collision/CollisionManager.h"
#include "Application/Enemy/Enemy.h"
#include "Application/Enemy/EnemySpawner.h"
#include "Application/MapChip.h"
#include "Application/Player/Player.h"

#include "EnemyLandingEmitter.h"
#include "PlayerMoveEmitter.h"
#include "EnemyDivideEmitter.h"
#include "BackGroundStarEmitter.h"
#include "EnemyDeadEmitter.h"

class Stage
{
public:
	Stage() = default;
	~Stage() {}

	void Initialize();
	void Update(Camera* camera);
	void DrawModels();

	void Debug();
  
	void UpdateBackGround();
	void DrawBackGround();

	void UpdatePlayerAndMapChip(Camera* camera);
	void UpdateEnemies();

	/// <summary>
	/// ステータスの初期化 (model の読み込み などをしない初期化)
	/// </summary>
	void InitializeStatus(const std::string& scene);
private:
	void CheckAlCollisions();
private:
	/// マップチップ
	ModelManager::ModelData modelBlock_;
	std::unique_ptr<MapChipField> mapChip_;

	// enemy
	ModelManager::ModelData enemyModel;
	int32_t enemySpawnerValue_;
	std::vector<std::unique_ptr<EnemySpawner>> enemySpawners_;

	std::list<std::unique_ptr<Enemy>> enemies_;

	// player
	std::unique_ptr<Player> player_;

	// hand
	std::array<ModelManager::ModelData,2> sideHandModel_;
	std::array<std::unique_ptr<Object3D>,2> sideHandObject_;

	// timer
	ModelManager::ModelData timerModel_;
	ModelManager::ModelData timerNeedleModel_;
	std::unique_ptr<Object3D> timerObject_;
	std::unique_ptr<Object3D> timerNeedleObject_;
	Float3 timerNeedleStartRotate_;

	std::unique_ptr<CollisionManager> collisionManager_;

	// 数字
	ModelManager::ModelData numberModel_[10];
	std::unique_ptr<Object3D> numberObject_[10][3];
	
	std::unique_ptr<Object3D> timerNumberObject_[10][2];

	// パーセント
	ModelManager::ModelData percentModel_;
	std::unique_ptr<Object3D> percentObject_;

	float chargedEnergy_;
	float maxEnergy_;

	float limitTime_;
	float leftTime_;

	bool isClear_;
	bool isGameOver_;
public:
	bool GetIsClear()const { return isClear_; }
	bool GetIsGameOver()const { return isGameOver_; }

	float GetMaxEnergy()const { return maxEnergy_; }
	float GetChargedEnergy()const { return chargedEnergy_; }

	float GetLimitTime()const { return limitTime_; }
	float GetLeftTime()const { return leftTime_; }

	void SetEnergy(float energy) { chargedEnergy_ = energy; }
	void SetMaxEnergy(float maxEnergy) { maxEnergy_ = maxEnergy; }

	Player* GetPlayer() { return player_.get(); }
	std::list<std::unique_ptr<Enemy>>& GetEnemies() { return enemies_; }

	void ClearEnemies();

	/// <summary>
	/// パワーをチャージ
	/// </summary>
	/// <param name="power">チャージ量</param>
	void ChargeEnergy(float energy) { chargedEnergy_ += energy; }
	float StealEnergy(float stealEnergy)
	{
	// 総量 が 奪われる 量より多かったら
		if(chargedEnergy_ <= stealEnergy)
		{
			// 全部 奪われる
			chargedEnergy_ = 0.0f;
			return chargedEnergy_;
		};
		chargedEnergy_ -= stealEnergy;
		return stealEnergy;
	}

private:
	// エネミー着地時パーティクルのエミッター
	EnemyLandingEmitter enemyLandingEmitter_;
	// エネミー着地時パーティクルのモデル
	ModelManager::ModelData modelEnemyLandingParticle_;

	// プレイヤー移動時パーティクルのエミッター
	PlayerMoveEmitter playerMoveEmitter_;
	// プレイヤー移動時パーティクルのモデル
	ModelManager::ModelData modelPlayerMoveParticle_;

	// エネミー分裂時パーティクルのエミッター
	EnemyDivideEmitter enemyDivideEmitter_;
	// エネミー分裂時パーティクルのモデル
	ModelManager::ModelData modelEnemyDivideParticle_;

	// エネミー死亡時パーティクルのエミッター
	EnemyDeadEmitter enemyDeadEmitter_;
	// エネミー死亡時パーティクルのモデル
	ModelManager::ModelData modelEnemyDeadParticle_;

	// 背景の星パーティクル(2D)
	BackGroundStarEmitter backGroundStarEmitter_;

private:
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	// 背景スプライト
	std::unique_ptr<Sprite> backGroundSprite_;
	// パーティクルの発生に使用するタイマー
	int32_t emitTimer_;

	// 背景の雲スプライト
	std::unique_ptr<Sprite[]> cloudSprite_;
	float near0x, near1x;
	float far2x, far3x;
	float cloudY = 0.0f; // 雲スプライトに全て共通で持たせる
	float nearCloudMoveSpeed = 1.0f;
	float farCloudMoveSpeed = 0.6f;

	// 背景の雲を更新
	void UpdateCloudPosition(Sprite& sprite, float& x, float moveSpeed, float resetThreshold, float resetPosition);

public:
	// 背景の雲を画面上へ移動させる（ゲームクリア->タイトル移行時に呼ぶ）
	void UpBackGroundCloud();
	// 背景の雲を画面下へ移動させる（ゲームオーバー->タイトル移行時に呼ぶ）
	void DownBackGroundCloud();
	// 背景の雲のY座標が正しい位置ではない場合に元の位置に戻す（タイトル画面で呼ぶ）
	void RestoreBackGroundCloud();

	const float kCloudUpSpeed = 14.0f;
	const float kCloudDownSpeed = 12.0f;

	float GetCloudY() { return cloudY; }
};