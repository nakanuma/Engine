#include "GamePlayScene.h" 
#include "ImguiWrapper.h" 
#include "DirectXBase.h"
#include "SRVManager.h"
#include "SpriteCommon.h"

void GamePlayScene::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera = std::make_unique<Camera>(Float3{0.0f, 15.0f, -40.0f}, Float3{0.3f, 0.0f, 0.0f}, 0.45f);
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

	// LightManagerの初期化
	lightManager = LightManager::GetInstance();
	lightManager->Initialize();

	///
	///	↓ ゲームシーン用 
	///	
	
	// Texture読み込み
	uint32_t whiteGH = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	// モデルの読み込みとテクスチャの設定
	modelSphere_ = ModelManager::LoadModelFile("resources/Models", "sphere.obj", dxBase->GetDevice());
	modelSphere_.material.textureHandle = whiteGH;

	// オブジェクトの生成とモデル設定
	objectSphere_ = std::make_unique<Object3D>();
	objectSphere_->model_ = &modelSphere_;
	objectSphere_->transform_.translate = {0.0f, 4.0f, 0.0f};

	// 箱（地面）モデルとオブジェクトの生成
	modelCube_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	modelCube_.material.textureHandle = whiteGH;

	objectCube_ = std::make_unique<Object3D>();
	objectCube_->model_ = &modelCube_;
	objectCube_->transform_.scale = {10.0f, 1.0f, 10.0f};

}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update() { 
	objectSphere_->UpdateMatrix(); 

	objectCube_->UpdateMatrix();
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
	// ライトの定数バッファを設定
	lightManager->TransferContantBuffer();

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	/// 

	// オブジェクトの描画
	objectSphere_->Draw();

	objectCube_->Draw();

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
	// ライティング確認用スフィア
	ImGui::Text("Sphere");
	ImGui::DragFloat3("Sphere.translation", &objectSphere_->transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Sphere.rotation", &objectSphere_->transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Sphere.scale", &objectSphere_->transform_.scale.x, 0.01f);
	
	// カメラ
	ImGui::Text("Camera");
	ImGui::DragFloat3("Camera.translation", &camera->transform.translate.x, 0.01f);
	ImGui::DragFloat3("Camera.rotation", &camera->transform.rotate.x, 0.01f);

	// ライト
	ImGui::Text("Light");
	ImGui::DragFloat3("SpotLight.position", &lightManager->spotLightsCB_.data_->spotLights[0].position.x, 0.01f);

	ImGui::End();

	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}
