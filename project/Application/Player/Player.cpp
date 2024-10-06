#include "Player.h"
#include "PlayerState.h"

#include "DirectXBase.h"
#include "externals/imgui/imgui.h"

void Player::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	modelData_ = ModelManager::LoadModelFile("./resources/Models","monkey.obj",dxBase->GetDevice());
	object_ = std::make_unique<Object3D>();	
	object_->model_ = &modelData_;

	TransitionState(new NeutralPlayerState(this));
}

void Player::Update()
{
	currentState_->Update();
	object_->UpdateMatrix();
}

void Player::Draw()
{
	ImGui::DragFloat3("Translate",&object_->transform_.translate.x,0.1f);
	object_->Draw();
}

void Player::TransitionState(IPlayerState* state)
{
	currentState_.reset(state);
	currentState_->Initialize();
}
