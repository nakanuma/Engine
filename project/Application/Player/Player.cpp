#include "Player.h"
#include "PlayerState.h"

#include "DirectXBase.h"
#include "GlobalVariables.h"
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG

#include "Application/Stage/Stage.h"

#include <numbers>

Player::~Player()
{
}

void Player::Initialize(uint32_t uvCheckerGH)
{
///===========================================================================================
/// GlobalVariables
///===========================================================================================
	GlobalVariables* variables = GlobalVariables::getInstance();
	// TODO
	// 要対策
	variables->addValue("Game","Wave","minRange_",minWaveRange_);
	variables->addValue("Game","Wave","maxRange_",maxWaveRange_);
	variables->addValue("Game","Wave","initialYVelocity_",initialYVelocity_);
	variables->addValue("Game","Player_AttackState","minChargingEnergy_",minChargingEnergy_);
	variables->addValue("Game","Player_AttackState","maxChargingEnergy_",maxChargingEnergy_);
	variables->addValue("Game","Player_NeutralState","startPos_",startPos_);

///===========================================================================================
/// Hand
///===========================================================================================
	DirectXBase* dxBase = DirectXBase::GetInstance();
	handModelData_ = ModelManager::LoadModelFile("./resources/Models","player.obj",dxBase->GetDevice());
	handModelData_.material.textureHandle = TextureManager::Load("resources/Images/player.png",dxBase->GetDevice());
	handObject_ = std::make_unique<Object3D>();
	handObject_->transform_.translate = startPos_;
	handObject_->model_ = &handModelData_;
	// handObject_.parent = &body;

	auto onCollision = []([[maybe_unused]] Collider* a) {};

	auto onCollisionMapChip = [this](MapChipField::MapObject* mapObj)
	{
		// wave を 起こす
		MapChipField::IndexSet address = mapObj->GetIndexSet();
		float waveRange = Lerp(chargePercent_,minWaveRange_,maxWaveRange_);
		mapChipField_->TriggerWave(address.xIndex,address.zIndex,waveRange,initialYVelocity_);

		// Energy を 貯める
		float chargingEnergy = Lerp(chargePercent_,minChargingEnergy_,maxChargingEnergy_);
		stage_->ChargeEnergy(chargingEnergy);
		chargePercent_ = 0.0f;


		TransitionState(new NeutralPlayerState(this));
	};
	handCollider_ = std::make_unique<Collider>();
	handCollider_->Init(handObject_->transform_.translate,0.3f,onCollision,onCollisionMapChip);

///===========================================================================================
/// State
///===========================================================================================
	TransitionState(new NeutralPlayerState(this));

///===========================================================================================
/// Sound
///===========================================================================================
	SoundManager* soundManager = SoundManager::GetInstance();
	attackSound_ = soundManager->LoadWave("resources/Sounds/attack.wav");
	chargeSound_ = soundManager->LoadWave("resources/Sounds/charge.wav");
}

void Player::InitializeStatus()
{
///===========================================================================================
/// GlobalVariables
///===========================================================================================
	GlobalVariables* variables = GlobalVariables::getInstance();
	// TODO
	// 要対策
	variables->addValue("Game","Wave","minRange_",minWaveRange_);
	variables->addValue("Game","Wave","maxRange_",maxWaveRange_);
	variables->addValue("Game","Wave","initialYVelocity_",initialYVelocity_);
	variables->addValue("Game","Player_AttackState","minChargingEnergy_",minChargingEnergy_);
	variables->addValue("Game","Player_AttackState","maxChargingEnergy_",maxChargingEnergy_);
	variables->addValue("Game","Player_NeutralState","startPos_",startPos_);

///===========================================================================================
/// Hand
///===========================================================================================
	auto onCollision = []([[maybe_unused]] Collider* a) {};

	auto onCollisionMapChip = [this](MapChipField::MapObject* mapObj)
	{
		// wave を 起こす
		MapChipField::IndexSet address = mapObj->GetIndexSet();
		float waveRange = Lerp(chargePercent_,minWaveRange_,maxWaveRange_);
		mapChipField_->TriggerWave(address.xIndex,address.zIndex,waveRange,initialYVelocity_);

		// Energy を 貯める
		float chargingEnergy = Lerp(chargePercent_,minChargingEnergy_,maxChargingEnergy_);
		stage_->ChargeEnergy(chargingEnergy);
		chargePercent_ = 0.0f;
	};
	handObject_->transform_.translate = startPos_;
	handCollider_ = std::make_unique<Collider>();
	handCollider_->Init(handObject_->transform_.translate,0.3f,onCollision,onCollisionMapChip);

///===========================================================================================
/// State
///===========================================================================================
	TransitionState(new NeutralPlayerState(this));
}

void Player::Update()
{
	currentState_->Update();
	handObject_->UpdateMatrix();

	handCollider_->SetPosition(handObject_->transform_.translate);

	// 移動制限一旦書いときました(他に書くところがあるなら消してもいいよ!!)
	// この範囲がマップの叩ける範囲
	if (handObject_->transform_.translate.x >= 39) {
		handObject_->transform_.translate.x = 39;
	}
	if (handObject_->transform_.translate.x <= -0) {
		handObject_->transform_.translate.x = -0;
	}
	if (handObject_->transform_.translate.z >= 39) {
		handObject_->transform_.translate.z = 39;
	}
	if (handObject_->transform_.translate.z <= -0) {
		handObject_->transform_.translate.z = -0;
	}
}

void Player::Draw()
{
	handObject_->Draw();
}

void Player::TransitionState(IPlayerState* state)
{
	currentState_.reset(state);
	currentState_->Initialize();
}

void Player::PlayAttackSound()
{
	SoundManager::GetInstance()->PlayWave(attackSound_,false,1.4f);
}

void Player::PlayChargeSound()
{
	SoundManager::GetInstance()->PlayWave(chargeSound_,true,0.3f);
}

void Player::StopChargeSound()
{
	SoundManager::GetInstance()->StopWave(chargeSound_);
}
