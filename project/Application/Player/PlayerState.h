#pragma once

#include "Input.h"

#include "Float2.h"
#include "Float3.h"

struct MoveKeys
{
	BYTE up[2] = {DIK_UP,DIK_W};
	BYTE down[2] = {DIK_DOWN,DIK_S};
	BYTE left[2] = {DIK_LEFT,DIK_A};
	BYTE right[2] = {DIK_RIGHT,DIK_D};
	BYTE attack = DIK_SPACE;
};
const MoveKeys moveKeys{};

class Player;
class IPlayerState
{
public:
	IPlayerState(Player* player):player_(player)
	{
		input_ = Input::GetInstance();
	}
	virtual ~IPlayerState() {}

	virtual void Initialize() = 0;
	virtual void Update() = 0;

protected:
	Player* player_;
	Input* input_;
};

///======================================================
/// 通常状態
///======================================================
class NeutralPlayerState
	:public IPlayerState
{
public:
	NeutralPlayerState(Player* player):IPlayerState(player) {}
	~NeutralPlayerState()override;

	void Initialize()override;
	void Update()    override;
private:
	float defaultPosY_;
	float speed_;
};

///======================================================
/// チャージ 状態
///======================================================
class ChargePlayerState
	:public IPlayerState
{
public:
	ChargePlayerState(Player* player,const Float3& beforePos);
	~ChargePlayerState()override;

	void Initialize()override;
	void Update()override;

private:
	float maxTime_;
	// 最低限の 硬直 時間
	float minTime_;

	float currentTime_;

	float movedPosY_;
	Float3 beforePos_;
};

///======================================================
/// 攻撃 状態
///======================================================
class AttackPlayerState
	:public IPlayerState
{
public:
	AttackPlayerState(Player* player,float chargePercent);
	~AttackPlayerState()override;

	void Initialize()override;
	void Update()    override;
private:
	float chargePercent_;

	float maxTime_;
	float currentTime_;
	
	float maxSpeed_;
	float accel_;

	// chargePercent で 決まった Max Speed
	float chargedMaxSpeed_;
	float speed_;
};
