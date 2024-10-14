#include "Player.h"
#include "PlayerState.h"

#include "DirectXBase.h"
#include "GlobalVariables.h"
#include "externals/imgui/imgui.h"

Player::~Player()
{

}

void Player::Initialize(uint32_t uvCheckerGH)
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
///===========================================================================================
/// Body
///===========================================================================================
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
		MapChipField::IndexSet address = mapObj->GetIndexSet();
		mapChipField_->SetAmplitude(address.zIndex,address.xIndex,1.6f);
	};
	handCollider_ = std::make_unique<Collider>();
	handCollider_->Init(handObject_->transform_.translate,2.0f,onCollision,onCollisionMapChip);

///===========================================================================================
/// GlobalVariables
///===========================================================================================


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
	bodyObject_->Draw();

	handObject_->Draw();
}

void Player::TransitionState(IPlayerState* state)
{
	currentState_.reset(state);
	currentState_->Initialize();
}
