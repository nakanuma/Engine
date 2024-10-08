#include "PlayerState.h"
#include "Player.h"

#include <numbers>

#include "DeltaTime.h"
#include "GlobalVariables.h"


const float blockSize = 1.0f;

template <typename T>
T Lerp(const float& t,const T& start,const T& end)
{
	return static_cast<T>(((1.0 - t) * static_cast<float>(start)) + (t * static_cast<float>(end)));
}

Float2 Lerp(float t,const Float2& startPos,const Float2& endPos)
{
	Float2 lerp;
	lerp.x = Lerp<float>(t,startPos.x,endPos.x);
	lerp.y = Lerp<float>(t,startPos.y,endPos.y);

	return lerp;
}

float customEasing(float t)
{
	if(t < 0.4f)
	{
// 0から0.3の間で0から1まで上昇
		return t / 0.3f;
	} else if(t < 0.6f)
	{
// 0.3から0.6の間は1を維持
		return 1.0f;
	} else
	{
  // 0.6から1の間で1から0まで下降
		return 1.0f - ((t - 0.6f) / 0.4f);
	}
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

//=====================================================
/// Neutral 
void NeutralPlayerState::Initialize()
{
}

void NeutralPlayerState::Update()
{
	Float2 moveVal {};
	for(int32_t i = 0; i < 2; i++)
	{
		moveVal += {
			static_cast<float>(input_->PushKey(moveKeys.right[i]) - input_->PushKey(moveKeys.left[i])),
			static_cast<float>(input_->PushKey(moveKeys.up[i]) - input_->PushKey(moveKeys.down[i]))
		};
	}
	
	if(moveVal.x != 0.0f || moveVal.y != 0.0f)
	{
		player_->TransitionState(new MovingPlayerState(player_,moveVal));
		return;
	}
}
//=====================================================

//=====================================================
/// Moving 
MovingPlayerState::MovingPlayerState(Player* player,Float2 moveVal):IPlayerState(player)
{
	Float3 playerWorldPos = player_->GetWorldPosition();
	// 現在の Address を 計算
	fromAddress_ = Float2(
		playerWorldPos.x / blockSize,
		playerWorldPos.z / blockSize
	);
	// 移動後の Address を 計算
	forAddress_ = fromAddress_ + moveVal;

	// beforeRotate_ = player_->GetBodyRotate().y;
	// afterRotate_ = std::atan2(moveVal.x,moveVal.y);

	const Float3& currentRotate = player_->GetBodyRotate();
	player_->SetBodyRotate({currentRotate.x,std::atan2(moveVal.x,moveVal.y),currentRotate.z});
}

MovingPlayerState::~MovingPlayerState()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->DestroyItem("Game","Player_MovingState","fullTime");
	variables->DestroyItem("Game","Player_MovingState","jumpHeight");
}

void MovingPlayerState::Initialize()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Player_MovingState","fullTime",fullTime_);
	variables->addValue("Game","Player_MovingState","jumpHeight",jumpHeight_);
	currentTime_ = 0.0f;
}

void MovingPlayerState::Update()
{
	currentTime_ += DeltaTime::getInstance()->getDeltaTime();
	if(currentTime_ >= fullTime_)
	{
		player_->SetBodyTranslate(Float3(forAddress_.x,0.0f,forAddress_.y));

		const Float3& currentRotate = player_->GetBodyRotate();
		// player_->SetBodyRotate({currentRotate.x,afterRotate_,currentRotate.z});

		player_->TransitionState(new NeutralPlayerState(player_));
		return;
	}

	float t = currentTime_ / fullTime_;
	Float2 currentXZ = Lerp(t,fromAddress_,forAddress_);
	float currentY = Lerp<float>(customEasing(t),0.0f,jumpHeight_);

	player_->SetBodyTranslate(Float3(currentXZ.x,currentY,currentXZ.y));
	
	//const Float3& currentRotate = player_->GetBodyRotate();
	//player_->SetBodyRotate({currentRotate.x,LerpShortAngle(beforeRotate_,afterRotate_,t),currentRotate.z});
}
//=====================================================


//=====================================================
/// Charge 
ChargePlayerState::ChargePlayerState(Player* player):IPlayerState(player)
{


}
ChargePlayerState::~ChargePlayerState()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->DestroyItem("Game","Player_ChargeState","fullTime");
	variables->DestroyItem("Game","Player_ChargeState","movedHandOffset_");
}

void ChargePlayerState::Initialize()
{
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("Game","Player_ChargeState","fullTime",fullTime_);
	variables->addValue("Game","Player_ChargeState","movedHandOffset_",movedHandOffset_);
}

void ChargePlayerState::Update()
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