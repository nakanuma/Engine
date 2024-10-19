#include "GameClearScene.h"
#include "SceneManager.h"

#include "DirectXBase.h"
#include "ImguiWrapper.h" 
#include "SpriteCommon.h"
#include "SRVManager.h"

#include "GlobalVariables.h"
#include "DeltaTime.h"

void GameClearScene::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	debugCamera = std::make_unique<DebugCamera>();
	debugCamera->Initialize();

	// SpriteCommonの生成と初期化
	spriteCommon = std::make_unique<SpriteCommon>();
	spriteCommon->Initialize(DirectXBase::GetInstance());

	// TextureManagerの初期化
	TextureManager::Initialize(dxBase->GetDevice(),SRVManager::GetInstance());

	// SoundManagerの初期化
	soundManager = std::make_unique<SoundManager>();
	soundManager->Initialize();

	// Inputの初期化
	input = Input::GetInstance();

	// LightManagerの初期化
	lightManager = LightManager::GetInstance();
	lightManager->Initialize();

	camera = SceneManager::GetInstance()->GetCamera();
	Camera::Set(camera);

	///
	///	↓ ゲームシーン用 
	///	

#ifdef _DEBUG
	stage_ = SceneManager::GetInstance()->GetStage();
	if(!stage_)
	{
		SceneManager::GetInstance()->CreateStage();
		stage_ = SceneManager::GetInstance()->GetStage();
	}
	stage_->Initialize();
#endif // _DEBUG

	currentUpdate_ = [this]() { this->EnterSceneUpdate(); };

	///===========================================================================================
	/// GlobalBariables
	///===========================================================================================
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("GameClear","Times","enterSceneMaxTime_",enterSceneMaxTime_);
	variables->addValue("GameClear","Times","outSceneMaxTime_",outSceneMaxTime_);
	leftTime_ = enterSceneMaxTime_;

	variables->addValue("GameClear","Camera","cameraPosWhenOutScene_",cameraPosWhenOutScene_);

	///===========================================================================================
	/// ClearTextPlane
	///===========================================================================================
	planeModel_ = ModelManager::LoadModelFile("resources/Models","plane.obj",dxBase->GetDevice());
	clearTextTextureIndex_ = TextureManager::Load("resources/Images/white.png",dxBase->GetDevice());

	planeModel_.material.textureHandle = clearTextTextureIndex_;

	clearTextPlane_ = std::make_unique<Object3D>();
	clearTextPlane_->model_ = &planeModel_;

	///===========================================================================================
	/// Camera
	///===========================================================================================
	cameraPosWhenInScene_ = camera->transform.translate;
}

void GameClearScene::Finalize()
{
}

void GameClearScene::Update()
{
	currentUpdate_();
	stage_->Update(camera);
}

void GameClearScene::Draw()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	SRVManager* srvManager = SRVManager::GetInstance();

	// 描画前処理
	dxBase->PreDraw();
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = {srvManager->descriptorHeap.heap_.Get()};
	dxBase->GetCommandList()->SetDescriptorHeaps(1,descriptorHeaps);
	// ImGuiのフレーム開始処理
	ImguiWrapper::NewFrame();
	// カメラの定数バッファを設定
	Camera::TransferConstantBuffer();
	// ライトの定数バッファを設定
	lightManager->TransferContantBuffer();

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	/// 

	stage_->DrawModels();
	clearTextPlane_->Draw();

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	/// 

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	///
	/// ↓ ここからスプライトの描画コマンド
	/// 

	///
	/// ↑ ここまでスプライトの描画コマンド
	/// 


#ifdef _DEBUG
	GlobalVariables::getInstance()->Update();
#endif // _DEBUG

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

	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

void GameClearScene::EnterSceneUpdate()
{
	leftTime_ -= DeltaTime::getInstance()->getDeltaTime();
	if(leftTime_ <= 0.0f)
	{
		currentUpdate_ = [this]() { this->SceneUpdate(); };
	}
}

void GameClearScene::SceneUpdate()
{
	if(input->TriggerKey(DIK_SPACE))
	{
		leftTime_ = outSceneMaxTime_;
		currentUpdate_ = [this]() { this->OutSceneUpdate(); };
	}
}

void GameClearScene::OutSceneUpdate()
{
	leftTime_ -= DeltaTime::getInstance()->getDeltaTime(); 
	float t = 1.0f - (leftTime_ / outSceneMaxTime_);

	camera->transform.translate = Float3::Lerp(cameraPosWhenInScene_,cameraPosWhenOutScene_,t);

	if(leftTime_ <= 0.0f)
	{ 
		SceneManager::GetInstance()->ChangeScene("TITLE");
		camera->transform.translate = cameraPosWhenOutScene_;
		return;
	}
}
