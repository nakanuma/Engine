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

#include "GlobalVariables.h"

void TitleScene::Initialize()
{
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

	lightManager = LightManager::GetInstance();
	lightManager->Initialize();

	camera = SceneManager::GetInstance()->GetCamera();

	///
	///	↓ ゲームシーン用
	///	
	stage_ = SceneManager::GetInstance()->GetStage();
	if(!stage_)
	{
		SceneManager::GetInstance()->CreateStage();
		stage_ = SceneManager::GetInstance()->GetStage();
	}
	stage_->Initialize();

	GlobalVariables* variables = GlobalVariables::getInstance();

	///===========================================================================================
	/// Texture 
	///===========================================================================================
	buttonTextureIndex_ = TextureManager::Load("./resources/Images/white.png",dxBase->GetDevice());

	///===========================================================================================
	/// UI 
	///===========================================================================================
	buttonUI_ = std::make_unique<UI>();
	buttonUI_->Init("Title","buttonUI",buttonTextureIndex_,spriteCommon.get());
	signT_ = 1.0f;
	auto buttonUiUpdate = [this](Sprite* sprite){
		if(t_ >= 1.0f)
		{
			signT_ = -1.0f;
		} else if(t_ <= -1.0f)
		{
			signT_ = 1.0f;
		}
		t_ += 0.1f * signT_;

		Float2 currentButtonOffset = Float2::Lerp(t_,-buttonUiOffset_,buttonUiOffset_);
		sprite->SetPosition(buttonUI_->GetPosition() + currentButtonOffset);
	};
	buttonUI_->setUpdate(buttonUiUpdate);
	variables->addValue("Title","buttonUI","buttonUiOffset_",buttonUiOffset_);
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	
	///
	///	シーン切り替え
	/// 

	// ENTERキーを押したら
	if (input->TriggerKey(DIK_SPACE)) {
		//// ゲームプレイシーン（次シーンを生成）
		//BaseScene* scene = new GamePlayScene();
		//// シーン切り替え依頼
		//SceneManager::GetInstance()->SetNextScene(scene);

		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
		return;
	}
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

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	/// 

	stage_->DrawModels();

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	/// 

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	///
	/// ↓ ここからスプライトの描画コマンド
	/// 

	buttonUI_->Draw();

	///
	/// ↑ ここまでスプライトの描画コマンド
	/// 


#ifdef _DEBUG
	GlobalVariables::getInstance()->Update();
#endif // _DEBUG

	ImGui::Begin("window");

	ImGui::Text("Trigger ENTER key to GamePlayScene");

	ImGui::End();

	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}
