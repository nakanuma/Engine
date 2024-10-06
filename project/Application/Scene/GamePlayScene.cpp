#include "GamePlayScene.h" 
#include "ImguiWrapper.h" 
#include "DirectXBase.h"
#include "SRVManager.h"
#include "SpriteCommon.h"

void GamePlayScene::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera = std::make_unique<Camera>(Float3{0.0f, 5.5f, -20.0f}, Float3{0.25f, 0.0f, 0.0f}, 0.45f);
	Camera::Set(camera.get()); // 現在のカメラをセット

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

	///
	///	↓ ゲームシーン用 
	///	
	
	// Texture読み込み
	uint32_t uvCheckerGH = TextureManager::Load("resources/Images/monsterball.png", dxBase->GetDevice());
	uint32_t grassGH = TextureManager::Load("resources/Images/grass.png", dxBase->GetDevice());

	// モデルの読み込みとテクスチャの設定
	model_ = ModelManager::LoadModelFile("resources/Models", "sphere.obj", dxBase->GetDevice());
	model_.material.textureHandle = uvCheckerGH;

	// オブジェクトの生成とモデル設定
	object_ = std::make_unique<Object3D>();
	object_->model_ = &model_;

	
	// 地形モデル読み込みとオブジェクト生成
	modelTerrain_ = ModelManager::LoadModelFile("resources/Models", "terrain.obj", dxBase->GetDevice());
	modelTerrain_.material.textureHandle = grassGH;

	objectTerrain_ = std::make_unique<Object3D>();
	objectTerrain_->model_ = &modelTerrain_;
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update() { 
	object_->UpdateMatrix(); 

	objectTerrain_->UpdateMatrix();
}

void GamePlayScene::Draw()
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

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	/// 

	// オブジェクトの描画
	object_->Draw();

	objectTerrain_->Draw();

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
	
	ImGui::Begin("window");
	// ライティング確認用モンスターボール
	ImGui::Text("monsterBall");
	ImGui::DragFloat3("monsterBall.translation", &object_->transform_.translate.x, 0.01f);
	ImGui::DragFloat3("monsterBall.rotation", &object_->transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("monsterBall.scale", &object_->transform_.scale.x, 0.01f);
	// モンスターボールのライト
	ImGui::Text("DirectionalLight");
	ImGui::DragFloat3("directionalLight.direction", &object_->directionalLightCB_.data_->direction.x, 0.01f);
	object_->directionalLightCB_.data_->direction = Float3::Normalize(object_->directionalLightCB_.data_->direction);
	ImGui::Text("PointLight");
	ImGui::DragFloat3("pointLight.position", &object_->pointLightCB_.data_->position.x, 0.01f);
	// カメラ
	ImGui::Text("camera");
	ImGui::DragFloat3("camera.translation", &camera->transform.translate.x, 0.01f);
	ImGui::DragFloat3("camera.rotation", &camera->transform.rotate.x, 0.01f);

	ImGui::End();

	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}
