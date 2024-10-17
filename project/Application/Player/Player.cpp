#include "Player.h"
#include "PlayerState.h"

#include "DirectXBase.h"
#include "GlobalVariables.h"
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG

#include "Application/Stage/Stage.h"

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

///===========================================================================================
/// Body
///===========================================================================================
	DirectXBase* dxBase = DirectXBase::GetInstance();
	bodyModelData_ = ModelManager::LoadModelFile("./resources/Models","monkey.obj",dxBase->GetDevice());
	bodyModelData_.material.textureHandle = uvCheckerGH;
	bodyObject_ = std::make_unique<Object3D>();
	bodyObject_->model_ = &bodyModelData_;
	bodyObject_->transform_.translate.y = 4.0f;

///===========================================================================================
/// Hand
///===========================================================================================
	handModelData_ = ModelManager::LoadModelFile("./resources/Models","monkey.obj",dxBase->GetDevice());
	handModelData_.material.textureHandle = uvCheckerGH;
	handObject_ = std::make_unique<Object3D>();
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
	};
	handCollider_ = std::make_unique<Collider>();
	handCollider_->Init(handObject_->transform_.translate,0.1f,onCollision,onCollisionMapChip);

///===========================================================================================
/// State
///===========================================================================================
	TransitionState(new NeutralPlayerState(this));
}

void Player::Update()
{
	currentState_->Update();
	bodyObject_->UpdateMatrix();
	handObject_->UpdateMatrix();

	handCollider_->SetPosition(handObject_->transform_.translate);
}

void Player::Draw()
{
	ImGui::DragFloat3("Rotate",&bodyObject_->transform_.rotate.x,0.1f);
	ImGui::DragFloat3("Translate",&bodyObject_->transform_.translate.x,0.1f);

	ImGui::DragFloat3("Hand.Translate", &handObject_->transform_.translate.x, 0.1f);
	bodyObject_->Draw();

	handObject_->Draw();
}

void Player::TransitionState(IPlayerState* state)
{
	currentState_.reset(state);
	currentState_->Initialize();
}
