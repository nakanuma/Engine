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
	variables->DestroyItem("Game","Player_NeutralState","defaultHandOffset");
}
//=====================================================
/// Neutral 
void NeutralPlayerState::Initialize()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Player_NeutralState","speed_",speed_);
	variables->addValue("Game","Player_NeutralState","defaultHandOffset",defaultHandOffset_);
}

void NeutralPlayerState::Update()
{
	if(input_->PushKey(moveKeys.attack))
	{
		player_->TransitionState(
			new ChargePlayerState(
				player_,
				player_->GetHandTranslate() - player_->GetBodyTranslate() // offset を 計算
			)
		);
		return;
	}

	Float2 moveVal{};
	for(int32_t i = 0; i < 2; i++)
	{
		moveVal += {
			static_cast<float>(input_->PushKey(moveKeys.right[i]) - input_->PushKey(moveKeys.left[i])),
				static_cast<float>(input_->PushKey(moveKeys.up[i]) - input_->PushKey(moveKeys.down[i]))
		};
	}
	moveVal = Float2::Normalize(moveVal);
	Float3 bodyTranslate = player_->GetBodyTranslate() + (Float3(moveVal.x,0.0f,moveVal.y) * speed_) * DeltaTime::getInstance()->getDeltaTime();
	player_->SetBodyTranslate(bodyTranslate);
	if(moveVal.x != 0.0f || moveVal.y != 0.0f)
	{
		player_->SetBodyRotate({0.0f,atan2(moveVal.x,moveVal.y),0.0f});
	}

	// 後ろ に 付いてくるような感じに 
	player_->SetHandTranslate(Lerp(0.27f,player_->GetHandTranslate(),defaultHandOffset_ + bodyTranslate));
}
//=====================================================

//=====================================================
/// Charge 
ChargePlayerState::ChargePlayerState(Player* player,const Float3& beforeHandOffset)
	:IPlayerState(player),beforeHandOffset_(beforeHandOffset)
{

}
ChargePlayerState::~ChargePlayerState()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->DestroyItem("Game","Player_ChargeState","maxTime");
	variables->DestroyItem("Game","Player_ChargeState","minTime");
	variables->DestroyItem("Game","Player_ChargeState","movedHandOffset_");
}

void ChargePlayerState::Initialize()
{

	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Player_ChargeState","maxTime",maxTime_);
	variables->addValue("Game","Player_ChargeState","minTime",minTime_);
	variables->addValue("Game","Player_ChargeState","movedHandOffset_",movedHandOffset_);
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

	Float3 playerRotate = player_->GetBodyRotate();
	if(moveVal.x != 0.0f || moveVal.y != 0.0f)
	{
		playerRotate.y = atan2(moveVal.x,moveVal.y);
		player_->SetBodyRotate(playerRotate);
	}
	
	Float3 currentHandOffset = TransformMatrix(Lerp(t,beforeHandOffset_,movedHandOffset_),Matrix::RotationY(playerRotate.y));
	// 手の位置更新
	player_->SetHandTranslate(player_->GetBodyTranslate() + currentHandOffset);
}
//=====================================================

AttackPlayerState::AttackPlayerState(Player* player,float chargePercent):IPlayerState(player)
{
	chargePercent_ = chargePercent;
	player_->SetChargePercent(chargePercent);
}

AttackPlayerState::~AttackPlayerState()
{
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
	if(player_->GetHandTranslate().y <= 0.0f)
	{
		return player_->TransitionState(new NeutralPlayerState(player_));
	}

	const float& deltaTime = DeltaTime::getInstance()->getDeltaTime();
	speed_ += accel_ * deltaTime;
	speed_ = (std::max)(speed_,chargedMaxSpeed_);

	Float3 playerHandTranslate = player_->GetHandTranslate();
	playerHandTranslate.y -= speed_;
	player_->SetHandTranslate(playerHandTranslate);
}
//=====================================================

//=====================================================
/// KnockBack
void KnockBackPlayerState::Initialize()
{
}

void KnockBackPlayerState::Update()
{
}

//=====================================================

