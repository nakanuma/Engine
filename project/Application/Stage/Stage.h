#pragma once

#include <memory>
#include <list>

#include "ModelManager.h"

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

class Stage
{
public:
	Stage() = default;
	~Stage(){}

	void Initialize();
	void Update(Camera* camera);
	void DrawModels();

private:
	void CheckAlCollisions();
private:
	/// マップチップ
	ModelManager::ModelData modelBlock_;
	std::unique_ptr<MapChipField> mapChip_;

	ModelManager::ModelData enemyModel;
	int32_t enemySpawnerValue_;
	std::vector<std::unique_ptr<EnemySpawner>> enemySpawners_;

	std::list<std::unique_ptr<Enemy>> enemies_;

	std::unique_ptr<Player> player_;

	std::unique_ptr<CollisionManager> collisionManager_;

	float chargedEnergy_;
	float maxEnergy_;

	float limitTime_;
	float currentTime_;
public:
	float GetMaxEnergy()const{return maxEnergy_;}
	float GetChargedEnergy()const{return chargedEnergy_;}

	float GetLimitTime()const{return limitTime_;}
	float GetCurrentTime()const{return currentTime_;}

	void SetEnergy(float energy){ chargedEnergy_ = energy;}
	/// <summary>
	/// パワーをチャージ
	/// </summary>
	/// <param name="power">チャージ量</param>
	void ChargeEnergy(float energy){ chargedEnergy_ += energy;}
	float StealEnergy(float stealEnergy){
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

// nakanuma追加分
public:
	Player* GetPlayer() { return player_.get(); }

	void Debug();

private:
	// エネミー着地時パーティクルのエミッター
	EnemyLandingEmitter enemyLandingEmitter_;
	// エネミー着地時パーティクルのモデル
	ModelManager::ModelData modelEnemyLandingParticle_;

	// プレイヤー移動時パーティクルのエミッター
	PlayerMoveEmitter playerMoveEmitter_;
	// プレイヤー移動時パーティクルのモデル
	ModelManager::ModelData modelPlayerMoveParticle_;
};