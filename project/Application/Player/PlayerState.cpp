#include "PlayerState.h"
#include "Player.h"

#include <numbers>

#include "DeltaTime.h"
#include "GlobalVariables.h"


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
}
//=====================================================
/// Neutral 
void NeutralPlayerState::Initialize()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Player_NeutralState","speed_",speed_);

}

void NeutralPlayerState::Update()
{
	if(input_->PushKey(moveKeys.attack))
	{
		player_->TransitionState(new ChargePlayerState(player_));
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

	if(moveVal.x == 0.0f && moveVal.y == 0.0f)
	{
		return;
	}

	Float3 bodyTranslate = player_->GetBodyTranslate() + (Float3(moveVal.x,0.0f,moveVal.y) * speed_) * DeltaTime::getInstance()->getDeltaTime();
	player_->SetBodyTranslate(bodyTranslate);
	player_->SetBodyRotate({0.0f,atan2(moveVal.x,moveVal.y),0.0f});
}
//=====================================================

//=====================================================
/// Charge 
ChargePlayerState::ChargePlayerState(Player* player)
	:IPlayerState(player),beforeHandOffset_(player->GetHandTranslate())
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
	float t = currentTime_ / maxTime_;

	if(currentTime_ >= minTime_)
	{
		if(!input_->PushKey(moveKeys.attack))
		{
			//player_->TransitionState(new AttackPlayerState(player_,t));
			player_->TransitionState(new NeutralPlayerState(player_));
			return;
		}
	}

	player_->SetHandTranslate(Lerp(t,beforeHandOffset_,movedHandOffset_));
}
//=====================================================

AttackPlayerState::~AttackPlayerState()
{
}

//=====================================================
/// Attack
void AttackPlayerState::Initialize()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Player_ChargeState","maxTime",maxTime_);
	variables->addValue("Game","Player_ChargeState","movedHandOffset_",movedHandOffset_);
}

void AttackPlayerState::Update()
{
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

