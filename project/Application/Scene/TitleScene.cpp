#include "TitleScene.h" 
#include "ImguiWrapper.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
//#include "GamePlayScene.h"

#include <cmath>
#include <numbers>
#include "MyMath.h"

#include "DeltaTime.h"
#include "GlobalVariables.h"

void TitleScene::Initialize() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// SpriteCommonの生成と初期化
	spriteCommon = std::make_unique<SpriteCommon>();
	spriteCommon->Initialize(DirectXBase::GetInstance());

	// TextureManagerの初期化
	TextureManager::Initialize(dxBase->GetDevice(), SRVManager::GetInstance());

	// SoundManagerの初期化
	soundManager = std::make_unique<SoundManager>();
	soundManager->Initialize();

	// Inputの初期化
	input = Input::GetInstance();

	// LightManagerの初期化
	lightManager = LightManager::GetInstance();
	lightManager->Initialize();
	// プレイヤーの手に割り当てる丸影を有効化
	lightManager->spotLightsCB_.data_->spotLights[0].isActive = true;
	lightManager->spotLightsCB_.data_->spotLights[0].decay = 0.5f;
	lightManager->spotLightsCB_.data_->spotLights[0].distance = 60;

	camera = SceneManager::GetInstance()->GetCamera();

	///
	///	↓ ゲームシーン用
	///
	
	///===========================================================================================
	/// GlobalVariables
	///===========================================================================================
	GlobalVariables* variables = GlobalVariables::getInstance();


	stage_ = SceneManager::GetInstance()->GetStage();
	if (!stage_) {
		SceneManager::GetInstance()->CreateStage();
		stage_ = SceneManager::GetInstance()->GetStage();
	}
	stage_->InitializeStatus("Title");

	///===========================================================================================
	/// Camera
	///===========================================================================================
	cameraPosWhenEnterScene_ = camera->transform.translate;

	///===========================================================================================
	/// Texture
	///===========================================================================================
	buttonTextureIndex_ = TextureManager::Load("./resources/Images/push_space.png", dxBase->GetDevice());

	///===========================================================================================
	/// Title
	///===========================================================================================
	titleTextModel_ = ModelManager::LoadModelFile("resources/Models", "Title.obj", dxBase->GetDevice());
	titleTextModel_.material.textureHandle = TextureManager::Load("resources/Images/title_color.png", dxBase->GetDevice());

	titleTextObject_ = std::make_unique<Object3D>();
	titleTextObject_->model_ = &titleTextModel_;

	///===========================================================================================
	/// UI
	///===========================================================================================
	buttonUI_ = std::make_unique<UI>();
	buttonUI_->Init("Title", "buttonUI", buttonTextureIndex_, spriteCommon.get());
	signT_ = 1.0f;
	// button
	variables->addValue("Title","buttonUI","buttonUiOffset_",buttonUiOffset_);
	// 各 Update
	buttonUpdateWhenEnterScene_ = [this](Sprite* sprite) {
		Float4 color = sprite->GetColor();
		color.w = Lerp(t_, 0.0f, 1.0f);
		color.w = std::clamp(color.w, 0.0f, 1.0f);
		sprite->SetColor(color);
	};
	buttonUpdateWhenSceneUpdate_ = [this](Sprite* sprite) {
		if (t_ >= 1.0f) {
			signT_ = -1.0f;
		} else if (t_ <= -1.0f) {
			signT_ = 1.0f;
		}

		Float2 currentButtonOffset = Float2::Lerp(t_, -buttonUiOffset_, buttonUiOffset_);
		sprite->SetPosition(buttonUI_->GetPosition() + currentButtonOffset);
	};
	buttonUpdateWhenOutScene_ = [this](Sprite* sprite) {
		Float4 color = sprite->GetColor();
		color.w = Lerp(1.0f - t_, 0.0f, 1.0f);
		color.w = std::clamp(color.w, 0.0f, 1.0f);
		sprite->SetColor(color);
	};

	buttonUI_->setUpdate(buttonUpdateWhenEnterScene_);

	///===========================================================================================
	/// cloudPlane_
	///===========================================================================================
	uint32_t cloudTexture[3];
	cloudTexture[0] = TextureManager::Load("resources/Images/cloud_1.png", dxBase->GetDevice());
	cloudTexture[1] = TextureManager::Load("resources/Images/cloud_2.png", dxBase->GetDevice());

	cloudSprite_[0] = std::make_unique<UI>();
	cloudSprite_[0]->Init("Title", "Cloud1", cloudTexture[0], spriteCommon.get());

	cloudSprite_[1] = std::make_unique<UI>();
	cloudSprite_[1]->Init("Title", "Cloud2", cloudTexture[1], spriteCommon.get());

	// time
	variables->addValue("Title", "Times", "enterSceneMaxTime_", enterSceneMaxTime_);
	variables->addValue("Title", "Times", "outSceneMaxTime_", outSceneMaxTime_);
	leftTime_ = 0.0f;

	// Camera
	variables->addValue("Title", "Camera", "cameraHomePosition", cameraHomePos_);

	// text Model
	variables->addValue("Title", "Text", "Scale", titleTextObject_->transform_.scale);
	variables->addValue("Title", "Text", "Rotate", titleTextObject_->transform_.rotate);
	variables->addValue("Title", "Text", "Translate", titleTextObject_->transform_.translate);

	///===========================================================================================
	/// Update State
	///===========================================================================================

	currentUpdate_ = [this]() { this->EnterSceneUpdate(); };
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	///
	///	シーン切り替え
	/// 

	stage_->UpdatePlayerAndMapChip(camera);
	currentUpdate_();
	// 背景の更新
	stage_->UpdateBackGround();
	buttonUI_->Update();
}

void TitleScene::Draw()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	SRVManager* srvManager = SRVManager::GetInstance();

	// 描画前処理
	dxBase->PreDraw();
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvManager->descriptorHeap.heap_.Get() };
	dxBase->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	// ImGuiのフレーム開始処理
	ImguiWrapper::NewFrame();
	// カメラの定数バッファを設定
	Camera::TransferConstantBuffer();
	// ライトの定数バッファを設定
	lightManager->TransferContantBuffer();

	// スプライト描画前処理
	spriteCommon->PreDraw();

	///
	///	↓ ここから背景スプライトの描画コマンド
	/// 

	stage_->DrawBackGround();

	///
	///	↑ ここまで背景スプライトの描画コマンド
	/// 

	// スプライト描画後処理
	spriteCommon->PostDraw();
	Camera::TransferConstantBuffer();
	lightManager->TransferContantBuffer();

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	/// 

	titleTextObject_->UpdateMatrix();
	titleTextObject_->Draw();
	stage_->DrawModels();

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	/// 

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	///
	/// ↓ ここから前景スプライトの描画コマンド
	/// 
	
	buttonUI_->Draw();

	///
	/// ↑ ここまで前景スプライトの描画コマンド
	/// 

#pragma region 丸影の設定

// spotLight[0]の位置をプレイヤーの手と同期
	lightManager->spotLightsCB_.data_->spotLights[0].position = stage_->GetPlayer()->GetTranslate();

#pragma endregion
#ifdef _DEBUG
	GlobalVariables::getInstance()->Update();
#endif // _DEBUG

#ifdef _DEBUG
	ImGui::Begin("Camera");

	ImGui::DragFloat3("Camera translation",&camera->transform.translate.x,0.1f);
	ImGui::DragFloat3("Camera rotate",&camera->transform.rotate.x,0.1f);
	ImGui::Text("fps : %.1f",ImGui::GetIO().Framerate);

	ImGui::End();

	ImGui::Begin("Stage Information");
	ImGui::Text("you can't input numbers");
	float maxEnergy = stage_->GetMaxEnergy();
	float currentEnergy = stage_->GetChargedEnergy();
	ImGui::InputFloat("MaxEnergy",&maxEnergy,0.0f,0.0f,"%.1f",ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("CurrentEnergy",&currentEnergy,0.0f,0.0f,"%.1f",ImGuiInputTextFlags_ReadOnly);

	float limitTime = stage_->GetLimitTime();
	float currentTime = stage_->GetCurrentTime();
	ImGui::InputFloat("LimitTime",&limitTime,0.0f,0.0f,"%.1f",ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("CurrentTime",&currentTime,0.0f,0.0f,"%.1f",ImGuiInputTextFlags_ReadOnly);

	ImGui::End();
#endif // _DEBUG

	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

void TitleScene::EnterSceneUpdate()
{
	leftTime_ += DeltaTime::getInstance()->getDeltaTime();
	t_ = leftTime_ / enterSceneMaxTime_;
	buttonUI_->Update();

	camera->transform.translate = Float3::Lerp(cameraPosWhenEnterScene_,cameraHomePos_,t_);

	if(leftTime_ >= enterSceneMaxTime_)
	{
		camera->transform.translate = cameraHomePos_;
		currentUpdate_ = [this](){ this->SceneUpdate(); };
		buttonUI_->setUpdate(buttonUpdateWhenSceneUpdate_);
		leftTime_ = 0.0f;
		t_        = 0.5f;
	}
}

void TitleScene::SceneUpdate()
{
	t_ += DeltaTime::getInstance()->getDeltaTime() * signT_;
	if(stage_->GetIsClear())
	{
		currentUpdate_ = [this]() { this->OutSceneUpdate(); };
		buttonUI_->setUpdate(buttonUpdateWhenOutScene_);
		t_ = 0.0f;
	}
}

void TitleScene::OutSceneUpdate()
{
	leftTime_ += DeltaTime::getInstance()->getDeltaTime();
	t_ = leftTime_ / outSceneMaxTime_;
	if(leftTime_ >= outSceneMaxTime_)
	{
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}