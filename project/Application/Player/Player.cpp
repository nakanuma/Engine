#include "Player.h"
#include "PlayerState.h"

#include "DirectXBase.h"
#include "GlobalVariables.h"
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG

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

///===========================================================================================
/// Hand
///===========================================================================================
	handModelData_ = ModelManager::LoadModelFile("./resources/Models","monkey.obj",dxBase->GetDevice());
	handModelData_.material.textureHandle = uvCheckerGH;
	handObject_ = std::make_unique<Object3D>();
	handObject_->model_ = &handModelData_;

	// handObject_.parent = &body;

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
