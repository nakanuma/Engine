#include "UI.h"

#include "TextureManager.h"
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG

#include "GlobalVariables.h"

void UI::Init(const std::string& scene,
			  const std::string& uiName,
			  std::uint32_t textureIndex,
			  SpriteCommon* spriteCommon){

#ifndef _DEBUG
	Float2 pos_;
	Float2 size_;
	Float4 color_ = {1.0f,1.0f,1.0f,1.0f};
	Float2 anchor_ = {0.0f,0.0f};
	bool isFlipX_ = false;
	bool isFlipY_ = false;
#endif // !_DEBUG
	///===========================================================================================
	/// Sprite 生成
	///===========================================================================================
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(spriteCommon,textureIndex);

	///===========================================================================================
	/// GlobalVariables
	///===========================================================================================
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue(scene,uiName,"pos_",pos_);

	variables->addValue(scene,uiName,"color_",color_);
	variables->addValue(scene,uiName,"anchor_",anchor_);
	variables->addValue(scene,uiName,"isFlipX_",isFlipX_);
	variables->addValue(scene,uiName,"isFlipY_",isFlipY_);
	variables->addValue(scene,uiName,"size_",size_);
	
	sprite_->SetPosition(pos_);
	sprite_->SetSize(size_);
	sprite_->SetColor(color_);
	sprite_->SetAnchorPoint(anchor_);
	sprite_->SetFlipX(isFlipX_);
	sprite_->SetFlipY(isFlipY_);
}

void UI::Update(){
#ifdef _DEBUG
	sprite_->SetPosition(pos_);
	sprite_->SetSize(size_);
	sprite_->SetColor(color_);
	sprite_->SetAnchorPoint(anchor_);
	sprite_->SetFlipX(isFlipX_);
	sprite_->SetFlipY(isFlipY_);
#endif // _DEBUG
	if(!update_){
		return;
	}

	update_(sprite_.get());
}

void UI::Draw(){
	sprite_->Draw();
}