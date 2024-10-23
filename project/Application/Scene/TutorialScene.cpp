#include "TutorialScene.h"

#include "ImguiWrapper.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "SpriteCommon.h"
#include "SceneManager.h"
//#include "GamePlayScene.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include "MyMath.h"

#include "DeltaTime.h"
#include "GlobalVariables.h"

void TutorialScene::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

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
	stage_->InitializeStatus("Tutorial");
	playerAttackNum_ = 0;
	enemyHurtNum_ = 0;

	///===========================================================================================
	/// Textures 
	///===========================================================================================
	std::string directory = "./resources/Images/tutorial/";
	for(int32_t i = 1; i <= 4; i++)
	{
		tutorialTextTextures_.push_back(TextureManager::Load(directory + "tutorial_" + std::to_string(i) + ".png",dxBase->GetDevice()));
	}
	tutorialTextTextures_.push_back(TextureManager::Load(directory + "tutorial_" + std::to_string(7) + ".png",dxBase->GetDevice()));

	for(int32_t i = 9; i <= 10; i++)
	{
		tutorialTextTextures_.push_back(TextureManager::Load(directory + "tutorial_" + std::to_string(i) + ".png",dxBase->GetDevice()));
	}
	for(int32_t i = 12; i <= 13; i++)
	{
		tutorialTextTextures_.push_back(TextureManager::Load(directory + "tutorial_" + std::to_string(i) + ".png",dxBase->GetDevice()));
	}

	for(size_t i = 0; i < 6; i++)
	{
		tutorialTaskGuidTextures_.push_back(TextureManager::Load(directory + "tutorial_5_" + std::to_string(i) + ".png",dxBase->GetDevice()));
	}
	
	tutorialTaskGuidTextures_.push_back(TextureManager::Load(directory + "tutorial_8" + ".png",dxBase->GetDevice()));

	for(size_t i = 0; i < 6; i++)
	{
		tutorialTaskGuidTextures_.push_back(TextureManager::Load(directory + "tutorial_11_" + std::to_string(i) + ".png",dxBase->GetDevice()));
	}

	currentTextTextureIndex_ = 0;

	currentTextTexture_ = std::make_unique<Sprite>();
	currentTaskGuidTexture_ = std::make_unique<Sprite>();
	currentTextTexture_->Initialize(spriteCommon.get(),tutorialTextTextures_.front());
	currentTextTexture_->SetAnchorPoint({0.5f,0.5f});
	currentTaskGuidTexture_->Initialize(spriteCommon.get(),tutorialTaskGuidTextures_.front());
	currentTaskGuidTexture_->SetAnchorPoint({0.5f,0.5f});

	///===========================================================================================
	/// Update State 
	///===========================================================================================
	currentUpdate_ = [this]() { this->SceneUpdate(); };

	 ///=============================///
	///    tutorial Text Updates    ///
   ///=============================///

	// この順番で 実行される
	tutorialTextUpdate_.push_back([this]() { return TextTextureUpdate(4); });

	tutorialTextUpdate_.push_back([this]() { return TextTextureUpdate(1); });

	tutorialTextUpdate_.push_back([this]() { return TextTextureUpdate(2); });

	tutorialTextUpdate_.push_back([this]() { return TextTextureUpdate(2); });

	 ///=============================///
	///    tutorial Task Updates    ///
   ///=============================///
	auto punchFloor = [this]()
	{
		bool isPrePlayerAttack = stage_->GetPlayer()->GetIsAttack();
		stage_->UpdatePlayerAndMapChip(camera);
		bool isPlayerAttack = stage_->GetPlayer()->GetIsAttack();
		if(isPrePlayerAttack && !isPlayerAttack)
		{
			++playerAttackNum_;
		}
		currentTaskGuidTexture_->SetTextureIndex(tutorialTaskGuidTextures_[playerAttackNum_]);
		return playerAttackNum_ >= 5;
	};
	auto attackForEnemy = [this]()
	{
		currentTaskGuidTexture_->SetTextureIndex(tutorialTaskGuidTextures_[5]);
		stage_->UpdatePlayerAndMapChip(camera);
		stage_->UpdateEnemies();

		for(auto& enemy : stage_->GetEnemies())
		{
			if(enemy->IsHurt())
			{
				
				return true;
			}
		}
		return false;
	};
	auto attackForEnemyMore = [this]()
	{
		stage_->UpdatePlayerAndMapChip(camera);
		stage_->UpdateEnemies();

		for(auto& enemy : stage_->GetEnemies())
		{
			if(enemy->IsHurt())
			{
				enemyHurtNum_ += static_cast<int32_t>(1);
			}
		}
		enemyHurtNum_ = std::min(enemyHurtNum_,5);
		currentTaskGuidTexture_->SetTextureIndex(tutorialTaskGuidTextures_[6 + enemyHurtNum_ - 1 ]);
		return enemyHurtNum_ >= 5;
	};
	/*auto chargingEnergyForMax = [this]() {
		stage_->Update(camera);

		return stage_->GetIsClear();
	};*/

	// この順番で 実行される
	tutorialTask_.push_back(punchFloor);
	tutorialTask_.push_back(attackForEnemy);
	tutorialTask_.push_back(attackForEnemyMore);
	//tutorialTask_.push_back(chargingEnergyForMax);

	///===========================================================================================
	/// GlobalVariables 
	///===========================================================================================
	GlobalVariables* variables = GlobalVariables::getInstance();

	// time
	variables->addValue("Tutorial","Times","enterSceneMaxTime_",enterSceneMaxTime_);
	variables->addValue("Tutorial","Times","outSceneMaxTime_",outSceneMaxTime_);
	leftTime_ = 0.0f;

	variables->addValue("Tutorial","TextTexture","translate",textTexturePos_);
	variables->addValue("Tutorial","TaskGuidTexture","translate",taskGuidTexturePos_);

	///===========================================================================================
	/// Sound 
	///===========================================================================================
	clickSound_ = soundManager->LoadWave("resources/Sounds/click.wav");
}

void TutorialScene::Finalize()
{
	stage_->GetEnemies().clear();
}

void TutorialScene::Update()
{
	///
	///	シーン切り替え
	/// 
	
	currentUpdate_();
	stage_->UpdateBackGround();

//#ifdef _DEBUG
	currentTextTexture_->SetPosition(textTexturePos_);
	currentTextTexture_->Update();
	currentTaskGuidTexture_->SetPosition(taskGuidTexturePos_);
	currentTaskGuidTexture_->Update();
//#endif // _DEBUG
}

void TutorialScene::Draw()
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

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	/// 

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	if(doTask_)
	{
		currentTaskGuidTexture_->Draw();
	} else
	{
		currentTextTexture_->SetTextureIndex(tutorialTextTextures_[currentTextTextureIndex_]);
		currentTextTexture_->Draw();
	}


	///
	/// ↓ ここからスプライトの描画コマンド
	/// 

	///
	/// ↑ ここまでスプライトの描画コマンド
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

	stage_->Debug();
#endif // _DEBUG

	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

void TutorialScene::TextTextureUpdate(uint32_t textureSum)
{
	if(input->TriggerKey(DIK_SPACE))
	{
		currentTextTextureIndex_ += 1;
		soundManager->PlayWave(clickSound_,false,0.7f);
		if(currentTextTextureIndex_ >= textureSum)
		{
			currentTextTextureIndex_ = textureSum;
			doTask_ = true;
			currentTextTextureIndex_ = 0;

			for(size_t i = 0; i < textureSum; i++)
			{
				if(!tutorialTextTextures_.empty())
				{  // 安全に pop_front()
					tutorialTextTextures_.pop_front();
				}
			}
			if(!tutorialTextUpdate_.empty())
			{  // 安全に pop_front()
				tutorialTextUpdate_.pop_front();
			}

		}
	} else if(input->TriggerKey(DIK_S) || input->TriggerKey(DIK_A))
	{
		soundManager->PlayWave(clickSound_,false,0.7f);
		currentTextTextureIndex_ = std::clamp(static_cast<int32_t>(currentTextTextureIndex_ - 1),0,static_cast<int32_t>(textureSum) - 1);
	}
}

void TutorialScene::EnterSceneUpdate()
{
	// スキップするか選ぶ
	if(leftTime_ >= enterSceneMaxTime_)
	{
		currentUpdate_ = [this]() { this->SceneUpdate(); };
		leftTime_ = 0.0f;
		t_ = 0.5f;
	}
}

void TutorialScene::SceneUpdate()
{
	if(tutorialTask_.empty() && tutorialTextUpdate_.empty()) // 修正
	{
		currentUpdate_ = [this]() { this->OutSceneUpdate(); };
		return;
	}

	if(doTask_)
	{
		if(tutorialTask_.front()())  // taskの実行
		{
			tutorialTask_.pop_front();
			tutorialTaskGuidTextures_.pop_front();
			if(!tutorialTaskGuidTextures_.empty())
			{  // チェックを追加
				currentTaskGuidTexture_->SetTextureIndex(tutorialTaskGuidTextures_.front());
			}
			doTask_ = false;
		}
	} else
	{
		if(!tutorialTextUpdate_.empty())
		{  // 安全に呼び出し
			tutorialTextUpdate_.front()();
		}
	}
}

void TutorialScene::OutSceneUpdate()
{
	leftTime_ += DeltaTime::getInstance()->getDeltaTime();
	stage_->UpdatePlayerAndMapChip(camera);
	t_ = leftTime_ / outSceneMaxTime_;
	if(leftTime_ >= outSceneMaxTime_)
	{
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}