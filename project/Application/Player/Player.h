#pragma once

#include <memory>

#include "ModelManager.h"
#include "Object3D.h"
#include "PlayerState.h"
#include "SoundManager.h"

#include "Application/Collision/Collider.h"

class Stage;
class MapChipField;

class Player
{
public:
	Player() = default;
	~Player();

	void Initialize(uint32_t uvCheckerGH);
	void InitializeStatus();
	void Update();
	void Draw();
private:
	Stage* stage_;
	float maxChargingEnergy_;
	float minChargingEnergy_;

	Float3 startPos_;
	Float3 velocity_;

	std::unique_ptr<IPlayerState> currentState_;

	ModelManager::ModelData handModelData_;
	std::unique_ptr<Object3D> handObject_;
	std::unique_ptr<Collider> handCollider_;

	Float3 defaultHandOffset_;

	// wave の 広さに 関係する
	float chargePercent_;
	// wave を 起こすため
	MapChipField* mapChipField_;
	// Wave の広さ
	float minWaveRange_,maxWaveRange_;
	// Y軸速度
	float initialYVelocity_;

	bool isAttack_;

	SoundManager::SoundData attackSound_;
	SoundManager::SoundData chargeSound_;
public:
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void SetStage(Stage* stage){stage_ = stage;}

	void TransitionState(IPlayerState* state);

	const Float3& GetTranslate()const { return handObject_->transform_.translate; }
	void SetTranslate(const Float3& translate) { handObject_->transform_.translate = translate; }

	Float3 GetWorldPosition()const
	{
		return Float3(
			handObject_->wvpCB_.data_->World.r[3][0],
			handObject_->wvpCB_.data_->World.r[3][1],
			handObject_->wvpCB_.data_->World.r[3][2]
		);
	}

	const Float3& GetRotate()const { return handObject_->transform_.rotate; }
	void SetRotate(const Float3& rotate) { handObject_->transform_.rotate = rotate; }

	Collider* GetHandCollider()const { return handCollider_.get(); }

	void SetChargePercent(float percent){chargePercent_ = percent;}

	bool GetIsAttack()const{return isAttack_;}
	void SetIsAttack(bool isAttack){isAttack_ = isAttack;}

	void PlayAttackSound();
	void PlayChargeSound();
	void StopChargeSound();
// nakanuma追加分
public:
	// 動いている最中かどうかを返す
	bool IsMoving() const { return isMoving_; }
	// 動いている最中かどうか設定
	void SetIsMoving(bool flag) { isMoving_ = flag; }

private:
	// プレイヤーが動いているかどうか
	bool isMoving_ = false;
};

