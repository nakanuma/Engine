#pragma once

#include "Input.h"

#include "Float2.h"

class Player;
class IPlayerState
{
public:
	IPlayerState(Player* player):player_(player)
	{
		input_ = Input::GetInstance();
	}
	virtual ~IPlayerState(){}

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
	~NeutralPlayerState()override{}

	void Initialize()override;
	void Update()override;
private:
};

///======================================================
/// 移動状態
///======================================================
class MovingPlayerState
	:public IPlayerState
{
public:
	MovingPlayerState(Player* player,Float2 moveVal);
	~MovingPlayerState()override;

	void Initialize()override;
	void Update()override;
private:
	Float2 fromAddress_; // 移動前 の アドレス
	Float2 forAddress_;  // 移動後 の アドレス

	float fullTime_;
	float currentTime_;

	float jumpHeight_;

};