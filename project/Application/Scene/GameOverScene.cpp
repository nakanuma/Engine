#include "GameOverScene.h"
#include "SceneManager.h"

#include "DirectXBase.h"
#include "ImguiWrapper.h" 
#include "SpriteCommon.h"
#include "SRVManager.h"

#include "GlobalVariables.h"
#include "DeltaTime.h"

void GameOverScene::Initialize()
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
	// プレイヤーの手に割り当てる丸影を有効化
	lightManager->spotLightsCB_.data_->spotLights[0].isActive = true;
	lightManager->spotLightsCB_.data_->spotLights[0].decay = 0.5f;
	lightManager->spotLightsCB_.data_->spotLights[0].distance = 60;

	camera = SceneManager::GetInstance()->GetCamera();

	///
	///	↓ ゲームシーン用 
	///	
	stage_ = SceneManager::GetInstance()->GetStage();

	currentUpdate_ = [this]() { this->EnterSceneUpdate(); };


	///===========================================================================================
	/// Camera
	///===========================================================================================
	cameraPosWhenEnterScene_ = camera->transform.translate;

	///===========================================================================================
	/// ClearTextPlane
	///===========================================================================================
	gameoverTextTextureIndex_ = TextureManager::Load("resources/Images/white.png",dxBase->GetDevice());
	gameoverTextPlane_ = std::make_unique<TexturePlane>();
	gameoverTextPlane_->Initialize("GameOver","GameOverText",gameoverTextTextureIndex_,dxBase->GetDevice());
	
	///===========================================================================================
	/// GlobalVariables
	///===========================================================================================
	GlobalVariables* variables = GlobalVariables::getInstance();
	variables->addValue("GameOver","Times","enterSceneMaxTime_",enterSceneMaxTime_);
	variables->addValue("GameOver","Times","outSceneMaxTime_"  ,outSceneMaxTime_);
	leftTime_ = enterSceneMaxTime_;

	variables->addValue("GameOver","Camera","cameraPosWhenOutScene_",cameraPosWhenOutScene_);
}

void GameOverScene::Finalize()
{
}

void GameOverScene::Update()
{
	stage_->UpdatePlayerAndMapChip(camera);
	currentUpdate_();
}

void GameOverScene::Draw()
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

	stage_->DrawModels();
	gameoverTextPlane_->Update();
	gameoverTextPlane_->Draw();

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	/// 

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	///
	/// ↓ ここから前景スプライトの描画コマンド
	/// 

	///
	/// ↑ ここまで前景スプライトの描画コマンド
	/// 

#pragma region 丸影の設定

// spotLight[0]の位置をプレイヤーの手と同期
	lightManager->spotLightsCB_.data_->spotLights[0].position = stage_->GetPlayer()->GetTranslate();

#pragma endregion
#ifdef _DEBUG
	GlobalVariables::getInstance()->Update();


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
	float currentTime = stage_->GetLeftTime();
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

void GameOverScene::EnterSceneUpdate()
{
	leftTime_ -= DeltaTime::getInstance()->getDeltaTime();
	float t = 1.0f - (leftTime_ / outSceneMaxTime_);
	gameoverTextPlane_->GetPlaneObject()->materialCB_.data_->color.w = Lerp(t,0.0f,1.0f);

	if(leftTime_ <= 0.0f)
	{
		gameoverTextPlane_->GetPlaneObject()->materialCB_.data_->color.w = 1.0f;

		stage_->ClearEnemies();

		currentUpdate_ = [this]() { this->SceneUpdate(); };
	}

	// 背景の更新
	stage_->UpdateBackGround();
}

void GameOverScene::SceneUpdate()
{
	if(input->TriggerKey(DIK_SPACE))
	{
		leftTime_ = outSceneMaxTime_;
		currentUpdate_ = [this]() { this->OutSceneUpdate(); };
	}

	// 背景の更新
	stage_->UpdateBackGround();
}

void GameOverScene::OutSceneUpdate()
{
	leftTime_ -= DeltaTime::getInstance()->getDeltaTime();
	float t = 1.0f - (leftTime_ / outSceneMaxTime_);

	camera->transform.translate = Float3::Lerp(cameraPosWhenEnterScene_,cameraPosWhenOutScene_,t);

	if(leftTime_ <= 0.0f)
	{
		SceneManager::GetInstance()->ChangeScene("TITLE");
		camera->transform.translate = cameraPosWhenOutScene_;
		return;
	}

	// 背景の雲を下へ移動
	stage_->DownBackGroundCloud();

	// 背景の更新
	stage_->UpdateBackGround();
}
