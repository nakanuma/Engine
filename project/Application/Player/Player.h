#pragma once

#include <memory>

#include "ModelManager.h"
#include "Object3D.h"
#include "PlayerState.h"

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

	Float3 velocity_;

	std::unique_ptr<IPlayerState> currentState_;

	// body
	ModelManager::ModelData bodyModelData_;
	std::unique_ptr<Object3D> bodyObject_;
	// hand
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
public:
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void SetStage(Stage* stage){stage_ = stage;}

	void TransitionState(IPlayerState* state);

	const Float3& GetBodyTranslate()const { return bodyObject_->transform_.translate; }
	void SetBodyTranslate(const Float3& translate) { bodyObject_->transform_.translate = translate; }
	const Float3& GetHandTranslate()const { return handObject_->transform_.translate; }
	void SetHandTranslate(const Float3& translate) { handObject_->transform_.translate = translate; }

	Float3 GetWorldPosition()const
	{
		return Float3(
			bodyObject_->wvpCB_.data_->World.r[3][0],
			bodyObject_->wvpCB_.data_->World.r[3][1],
			bodyObject_->wvpCB_.data_->World.r[3][2]
		);
	}

	const Float3& GetBodyRotate()const { return bodyObject_->transform_.rotate; }
	void SetBodyRotate(const Float3& rotate) { bodyObject_->transform_.rotate = rotate; }
	const Float3& GetHandRotate()const { return handObject_->transform_.rotate; }
	void SetHandRotate(const Float3& rotate) { handObject_->transform_.rotate = rotate; }

	Collider* GetHandCollider()const { return handCollider_.get(); }

	void SetChargePercent(float percent){chargePercent_ = percent;}

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

