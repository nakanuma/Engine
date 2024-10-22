#include "GamePlayScene.h" 
#include "SceneManager.h"

#include "ImguiWrapper.h" 
#include "DirectXBase.h"
#include "SRVManager.h"
#include "SpriteCommon.h"

#include "GlobalVariables.h"

void GamePlayScene::Initialize()
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

	// SRVManagerの生成と初期化
	srvManager = std::make_unique<SRVManager>();
	srvManager->Initialize(dxBase);

	// ParticleManagerの生成と初期化
	particleManager = std::make_unique<ParticleManager>();
	particleManager->Initialize(dxBase, srvManager.get());

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
	stage_->InitializeStatus("Game");

	soundData1 = soundManager->LoadWave("resources/Sounds/yay.wav");


	buttonUpdate_[0] = [this](Sprite* sprite) {
		bool isActive = Input::GetInstance()->PushKey(DIK_UP) || Input::GetInstance()->PushKey(DIK_W);
		// 各ボタンの位置とサイズを個別に指定
		Float2 position = { 500, 600 };
		Float2 size = { 50, 50 };
		ButtonSpriteUI(sprite, isActive, position, size);
		};

	buttonUpdate_[1] = [this](Sprite* sprite) {
		bool isActive = Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_A);
		Float2 position = { 450, 650 };
		Float2 size = { 50, 50 };
		ButtonSpriteUI(sprite, isActive, position, size);
		};

	buttonUpdate_[2] = [this](Sprite* sprite) {
		bool isActive = Input::GetInstance()->PushKey(DIK_DOWN) || Input::GetInstance()->PushKey(DIK_S);
		Float2 position = { 500, 650 };
		Float2 size = { 50, 50 };
		ButtonSpriteUI(sprite, isActive, position, size);
		};

	buttonUpdate_[3] = [this](Sprite* sprite) {
		bool isActive = Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_D);
		Float2 position = { 550, 650 };
		Float2 size = { 50, 50 };
		ButtonSpriteUI(sprite, isActive, position, size);
		};

	buttonUpdate_[4] = [this](Sprite* sprite) {
		bool isActive = Input::GetInstance()->PushKey(DIK_SPACE);
		Float2 position = { 640, 650 };
		Float2 size = { 120, 50 };
		ButtonSpriteUI(sprite, isActive, position, size);
		};

	uint32_t bottonTexture[5];
	bottonTexture[0] = TextureManager::Load("resources/Images/up.png", dxBase->GetDevice());
	bottonTexture[1] = TextureManager::Load("resources/Images/left.png", dxBase->GetDevice());
	bottonTexture[2] = TextureManager::Load("resources/Images/down.png", dxBase->GetDevice());
	bottonTexture[3] = TextureManager::Load("resources/Images/right.png", dxBase->GetDevice());
	bottonTexture[4] = TextureManager::Load("resources/Images/space.png", dxBase->GetDevice());
	///スプライト
	for (int i = 0; i < 5; i++) {
		buttonSprite_[i] = std::make_unique<UI>();
		buttonSprite_[i]->Init("Game", "ButtonUI", bottonTexture[i], spriteCommon.get());
		buttonSprite_[i]->setUpdate(buttonUpdate_[i]);
	}
	
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update()
{
#ifdef _DEBUG // デバッグカメラ
	DebugCameraUpdate(input);
#endif
	if(stage_->GetIsClear())
	{
		SceneManager::GetInstance()->ChangeScene("GAMECLEAR");
		return;
	} else if(stage_->GetIsGameOver())
	{
		SceneManager::GetInstance()->ChangeScene("GAMEOVER");
		return;
	}

	// パーティクルマネージャーの更新
	particleManager->Update();
  
	stage_->Update(camera);

	for (int i = 0; i < 5; i++) {
		buttonSprite_[i]->Update();
	}

	// 背景の更新
	stage_->UpdateBackGround();
}

void GamePlayScene::Draw()
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

	///
	/// ↓ ここから前景スプライトの描画コマンド
	/// 
	 
	for (int i = 0; i < 5; i++) {
		buttonSprite_[i]->Draw();
	}

	///
	/// ↑ ここまで前景スプライトの描画コマンド
	/// 

	#pragma region パーティクル用PSOに変更->パーティクル描画->通常PSOに変更
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineStateParticle());

	particleManager->Draw();

	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineState());
	#pragma endregion

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

	stage_->Debug();
#endif // _DEBUG

#ifdef _DEBUG // デバッグカメラ
	ImGui::Checkbox("useDebugCamera",&useDebugCamera);
#endif

#ifdef _DEBUG
	ImGui::DragFloat3("camera.rotation",&camera->transform.rotate.x,0.01f);

	ImGui::Text("fps : %.1f", ImGui::GetIO().Framerate);

	// 音声の再生
	if (ImGui::Button("PlaySound")) {
		soundManager->PlayWave(soundData1, false, 0.1f);
	}
	// 音声の停止
	if (ImGui::Button("StopSound")) {
		soundManager->StopWave(soundData1);
	}

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

	/*ImGui::Begin("Light");

	ImGui::DragFloat3("position", &lightManager->spotLightsCB_.data_->spotLights[0].position.x);
	ImGui::DragFloat("intensity", &lightManager->spotLightsCB_.data_->spotLights[0].intensity);
	ImGui::DragFloat3("direction", &lightManager->spotLightsCB_.data_->spotLights[0].direction.x);
	ImGui::DragFloat("distance", &lightManager->spotLightsCB_.data_->spotLights[0].distance);
	ImGui::DragFloat("decay", &lightManager->spotLightsCB_.data_->spotLights[0].decay);
	ImGui::DragFloat("cosAngle", &lightManager->spotLightsCB_.data_->spotLights[0].cosAngle);
	ImGui::DragFloat("cosFalloffStart", &lightManager->spotLightsCB_.data_->spotLights[0].cosFalloffStart);

	ImGui::End();*/

	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

#ifdef _DEBUG // デバッグカメラ
void GamePlayScene::DebugCameraUpdate(Input* input)
{
// 前回のカメラモード状態を保持
	static bool prevUseDebugCamera = false;

	// デバッグカメラが有効になった瞬間に通常カメラのTransformを保存
	if(useDebugCamera && !prevUseDebugCamera)
	{
		savedCameraTransform = camera->transform;
	}

	// デバッグカメラが有効の場合
	if(useDebugCamera)
	{
// デバッグカメラの更新
		debugCamera->Update(input);
		// 通常カメラにデバッグカメラのTransformを適用
		camera->transform = debugCamera->transform_;
	} else if(!useDebugCamera && prevUseDebugCamera)
	{
// 通常カメラのTransformを再現
		camera->transform = savedCameraTransform;
	}

	// 現在のカメラモードを保存して次のフレームで使う
	prevUseDebugCamera = useDebugCamera;
}

#endif

void GamePlayScene::ButtonSpriteUI(Sprite* sprite, bool isActive, const Float2& position, const Float2& size)
{
	// 色を設定
	Float4 color = isActive ? Float4{ 0.81f, 0.81f, 0.81f, 1 } : Float4{ 1, 1, 1, 1 };
	sprite->SetColor(color);

	// 位置を設定
	sprite->SetPosition(position);

	// サイズを設定
	sprite->SetSize(size);
}