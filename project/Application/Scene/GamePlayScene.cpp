#include "GamePlayScene.h" 
#include "ImguiWrapper.h"
#include "DirectXBase.h"
#include "SRVManager.h"
#include "SpriteCommon.h" 

void GamePlayScene::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera = std::make_unique<Camera>(Float3{ 0.0f, 0.0f, -10.0f }, Float3{ 0.0f, 0.0f, 0.0f }, 0.45f);
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
	uint32_t uvCheckerGH = TextureManager::Load("resources/Images/uvChecker.png", dxBase->GetDevice());

	// モデルの読み込みとテクスチャの設定
	model_ = ModelManager::LoadModelFile("resources/Models", "plane.obj", dxBase->GetDevice());
	model_.material.textureHandle = uvCheckerGH;

	// オブジェクトの生成とモデル設定
	object_ = std::make_unique<Object3D>();
	object_->model_ = &model_;
	object_->transform_.rotate = { 0.0f, 3.14f, 0.0f };


	

	// Texture読み込み
	uint32_t uvCheckerGHBlock = TextureManager::Load("resources/Images/uvChecker.png", dxBase->GetDevice());

	// モデルの読み込みとテクスチャの設定(マップチップ)
	modelBlock_ = ModelManager::LoadModelFile("resources/Models", "block.obj", dxBase->GetDevice());
	modelBlock_.material.textureHandle = uvCheckerGHBlock;

	// マップチップの設定
	GenerateBloks();

	// カメラ位置
	camera->transform.rotate = { 1.14f,0,0 };
	camera->transform.translate = { 20,50.0f,0 };
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update() {

	

	for (const std::vector<std::unique_ptr<Object3D>>& worldTransformBlockLine : objectBlocks_) {
		for (const std::unique_ptr<Object3D>& worldTransformBlock : worldTransformBlockLine) {
			worldTransformBlock->UpdateMatrix();
		}
	}
	object_->UpdateMatrix();
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


	// ブロック描画
	for (const std::vector<std::unique_ptr<Object3D>>& worldTransformBlockLine : objectBlocks_) {
		for (const std::unique_ptr<Object3D>& worldTransformBlock : worldTransformBlockLine) {
			worldTransformBlock->Draw();
		}
	}

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

	ImGui::DragFloat3("Camera translation", &camera->transform.translate.x, 0.1f);
	ImGui::DragFloat3("Camera rotate", &camera->transform.rotate.x, 0.1f);
	ImGui::DragFloat3("translation", &object_->transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotation", &object_->transform_.rotate.x, 0.01f);

	for (const std::vector<std::unique_ptr<Object3D>>& worldTransformBlockLine : objectBlocks_) {
		for (const std::unique_ptr<Object3D>& worldTransformBlock : worldTransformBlockLine) {
			ImGui::DragFloat3("translationBlock1", &worldTransformBlock->transform_.translate.x, 0.01f);
			
		}
	}

	

	ImGui::End();

	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

void GamePlayScene::GenerateBloks()
{
	// 要素数
	uint32_t kNumBlockVirtical = mapChip_->GetNumBlockVirtical();
	uint32_t kNumBlockHorizontal = mapChip_->GetNumBlockHorizontal();

	// ブロックの生成
	objectBlocks_.resize(kNumBlockVirtical); // 垂直方向のサイズを設定
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		objectBlocks_[i].resize(kNumBlockHorizontal); // 水平方向のサイズを設定
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			// ユニークポインタでObject3Dを初期化
			objectBlocks_[i][j] = std::make_unique<Object3D>();

			// モデルの設定
			objectBlocks_[i][j]->model_ = &modelBlock_;

			// 位置の設定
			objectBlocks_[i][j]->transform_.translate = mapChip_->GetMapChipPositionByIndex(j, i);
		}
	}
}