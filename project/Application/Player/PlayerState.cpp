#include "Player.h"
#include "PlayerState.h"

#include <cmath>
#include <numbers>

#include "DeltaTime.h"
#include "GlobalVariables.h"
#include "Matrix.h"

const float blockSize = 1.0f;

Float2 Lerp(float t,const Float2& startPos,const Float2& endPos)
{
	Float2 lerp;
	lerp.x = Lerp<float>(t,startPos.x,endPos.x);
	lerp.y = Lerp<float>(t,startPos.y,endPos.y);

	return lerp;
}
Float3 Lerp(float t,const Float3& startPos,const Float3& endPos)
{
	Float3 lerp;
	lerp.x = Lerp<float>(t,startPos.x,endPos.x);
	lerp.y = Lerp<float>(t,startPos.y,endPos.y);
	lerp.z = Lerp<float>(t,startPos.z,endPos.z);
	return lerp;
}

float LerpShortAngle(float before,float after,float t)
{
	constexpr float maxPi = std::numbers::pi_v<float> *2.0f;
	constexpr float minPi = std::numbers::pi_v<float> *-2.0f;

	float diff = after - before;

	float rotate = std::fmod(diff,maxPi);

	if(rotate >= std::numbers::pi)
	{
		rotate += minPi;
	} else if(rotate <= -std::numbers::pi)
	{
		rotate += maxPi;
	}

	return before + rotate * t;
}
NeutralPlayerState::~NeutralPlayerState()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->DestroyItem("Game","Player_NeutralState","speed_");
	variables->DestroyItem("Game","Player_NeutralState","defaultPosY_");
}
//=====================================================
/// Neutral 
void NeutralPlayerState::Initialize()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Player_NeutralState","speed_",speed_);
	variables->addValue("Game","Player_NeutralState","defaultPosY_",defaultPosY_);
}

void NeutralPlayerState::Update()
{

	Float2 moveVal{};
	for(int32_t i = 0; i < 2; i++)
	{
		moveVal += {
			static_cast<float>(input_->PushKey(moveKeys.right[i]) - input_->PushKey(moveKeys.left[i])),
				static_cast<float>(input_->PushKey(moveKeys.up[i]) - input_->PushKey(moveKeys.down[i]))
		};
	}
	moveVal = Float2::Normalize(moveVal);

	Float3 translate = player_->GetTranslate();
	translate += Float3(moveVal.x,0.0f,moveVal.y) * (speed_ * DeltaTime::getInstance()->getDeltaTime());
	translate.y = Lerp(0.145f,defaultPosY_,translate.y);

	player_->SetTranslate(translate);
	if(moveVal.x != 0.0f || moveVal.y != 0.0f)
	{
		player_->SetRotate({0.0f,atan2(moveVal.x,moveVal.y),0.0f});
		// 動いている状態
		player_->SetIsMoving(true);
	} else {
		// 動いていない状態
		player_->SetIsMoving(false);
	}

	if(translate.y - defaultPosY_ >= 0.1f)
	{
		return;
	}
	if(input_->PushKey(moveKeys.attack))
	{
		player_->TransitionState(
			new ChargePlayerState(
				player_,
				player_->GetTranslate() // offset を 計算
			)
		);
		return;
	}
}
//=====================================================

//=====================================================
/// Charge 
ChargePlayerState::ChargePlayerState(Player* player,const Float3& beforePos)
	:IPlayerState(player),beforePos_(beforePos)
{

}
ChargePlayerState::~ChargePlayerState()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->DestroyItem("Game","Player_ChargeState","maxTime");
	variables->DestroyItem("Game","Player_ChargeState","minTime");
	variables->addValue("Game","Player_ChargeState","movedPosY",movedPosY_);
}

void ChargePlayerState::Initialize()
{

	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Player_ChargeState","maxTime",maxTime_);
	variables->addValue("Game","Player_ChargeState","minTime",minTime_);
	variables->addValue("Game","Player_ChargeState","movedPosY",movedPosY_);
	currentTime_ = 0.0f;
}

void ChargePlayerState::Update()
{
	// 時間更新
	if(currentTime_ >= maxTime_)
	{
		currentTime_ = maxTime_;
	} else
	{
		currentTime_ += DeltaTime::getInstance()->getDeltaTime();
	}
	float t = currentTime_ / maxTime_;

	// 早期リターン
	if(currentTime_ >= minTime_)
	{
		if(!input_->PushKey(moveKeys.attack))
		{
			//player_->TransitionState(new AttackPlayerState(player_,t));
			player_->TransitionState(new AttackPlayerState(player_,t));
			return;
		}
	}

	// 方向更新
	Float2 moveVal{};
	for(int32_t i = 0; i < 2; i++)
	{
		moveVal += {
			static_cast<float>(input_->PushKey(moveKeys.right[i]) - input_->PushKey(moveKeys.left[i])),
			static_cast<float>(input_->PushKey(moveKeys.up[i]) - input_->PushKey(moveKeys.down[i]))
		};
	}
	moveVal = Float2::Normalize(moveVal);

	Float3 playerRotate = player_->GetRotate();
	if(moveVal.x != 0.0f || moveVal.y != 0.0f)
	{
		playerRotate.y = atan2(moveVal.x,moveVal.y);
		player_->SetRotate(playerRotate);
	} 
	player_->SetTranslate({beforePos_.x,Lerp(t,beforePos_.y,movedPosY_),beforePos_.z});
	// チャージ状態では常に動いてないことにする
	player_->SetIsMoving(false);
}
//=====================================================

AttackPlayerState::AttackPlayerState(Player* player,float chargePercent):IPlayerState(player)
{
	chargePercent_ = chargePercent;
	player_->SetChargePercent(chargePercent);
	player_->SetIsAttack(true);
}

AttackPlayerState::~AttackPlayerState()
{
	player_->SetIsAttack(false);
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->DestroyItem("Game","Player_AttackState","accel");
	variables->DestroyItem("Game","Player_AttackState","maxSpeed");
}

//=====================================================
/// Attack
void AttackPlayerState::Initialize()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Player_AttackState","accel",accel_);
	variables->addValue("Game","Player_AttackState","maxSpeed",maxSpeed_);

	chargedMaxSpeed_ = maxSpeed_ * chargedMaxSpeed_;
}

void AttackPlayerState::Update()
{
	// 地面に 衝突したら
	if(player_->GetTranslate().y <= -1.5f)
	{
		return player_->TransitionState(new NeutralPlayerState(player_));
	}

	const float& deltaTime = DeltaTime::getInstance()->getDeltaTime();
	speed_ += accel_ * deltaTime;
	speed_ = (std::max)(speed_,chargedMaxSpeed_);

	Float3 playerTranslate = player_->GetTranslate();
	playerTranslate.y -= speed_;
	player_->SetTranslate(playerTranslate);
}
//=====================================================
