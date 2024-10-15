#include "GamePlayScene.h" 
#include "ImguiWrapper.h" 
#include "DirectXBase.h"
#include "SRVManager.h"
#include "SpriteCommon.h"

#include "GlobalVariables.h"

void GamePlayScene::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera = std::make_unique<Camera>(Float3{10.0f, 20.0f, -30.0f}, Float3{0.44f, 0.0f, 0.0f}, 0.45f);
	Camera::Set(camera.get()); // 現在のカメラをセット

	// カメラのoriginalPositionに現在のカメラのtranslateをセット（シェイク時に使用、ずれを防止するために必要）
	camera->SetOriginalPosition(camera->transform.translate);

	// デバッグカメラの生成と初期化
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

	///
	///	↓ ゲームシーン用 
	///	

	// Texture読み込み
	uint32_t uvCheckerGH = TextureManager::Load("resources/Images/uvChecker.png",dxBase->GetDevice());

	// モデルの読み込みとテクスチャの設定
	model_ = ModelManager::LoadModelFile("resources/Models","plane.obj",dxBase->GetDevice());
	model_.material.textureHandle = uvCheckerGH;

	// オブジェクトの生成とモデル設定
	object_ = std::make_unique<Object3D>();
	object_->model_ = &model_;
	object_->transform_.rotate = {0.0f,3.14f,0.0f};

	// モデルの読み込みとテクスチャの設定(マップチップ)
	modelBlock_ = ModelManager::LoadModelFile("resources/Models","block.obj",dxBase->GetDevice());
	modelBlock_.material.textureHandle = uvCheckerGH;

	// マップチップ
	mapChip_ = std::make_unique<MapChipField>();

	// まずCSVファイルで読み込む
	mapChip_->LoadMapChipCsv("resources/blocks.csv");
	// そのあとに初期化
	mapChip_->Initialize(modelBlock_);

	object_->transform_.rotate = {0.0f, 3.14f, 0.0f};

	// Texture読み込み
	uint32_t monsterBallTexture = TextureManager::Load("resources/Images/monsterBall.png",dxBase->GetDevice());
	player_ = std::make_unique<Player>();
	player_->Initialize(monsterBallTexture);
	player_->SetMapChipField(mapChip_.get());

	GlobalVariables::getInstance()->addValue("Game","EnemySpawner_Default","spawnerValue",enemySpawnerValue_);
	enemyModel = ModelManager::LoadModelFile("resources/Models","block.obj",dxBase->GetDevice());
	enemyModel.material.textureHandle = monsterBallTexture;

	for(size_t i = 0; i < enemySpawnerValue_; ++i)
	{
		enemySpawners_.push_back(std::make_unique<EnemySpawner>());
		enemySpawners_.back()->Initialize(static_cast<int32_t>(enemySpawners_.size() - 1),&enemyModel);
		enemySpawners_.back()->SetEnemyModel(enemyModel);
	}

	// 衝突マネージャの生成
	collisionManager_ = std::make_unique<CollisionManager>();
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update()
{
#ifdef _DEBUG
	int32_t movingSpawnerValue = enemySpawnerValue_ - static_cast<int32_t>(enemySpawners_.size());

	if(movingSpawnerValue > 0)  // 増加する場合
	{
		for(int32_t i = 0; i < movingSpawnerValue; ++i)
		{
			enemySpawners_.push_back(std::make_unique<EnemySpawner>());
			enemySpawners_.back()->Initialize(static_cast<int32_t>(enemySpawners_.size() - 1),&enemyModel);
			enemySpawners_.back()->SetEnemyModel(enemyModel);
		}
	} else if(movingSpawnerValue < 0)  // 減少する場合
	{
		// 減少させる数だけポップバックする
		for(int32_t i = 0; i < abs(movingSpawnerValue); ++i)
		{
			if(!enemySpawners_.empty())
			{
				enemySpawners_.pop_back();
			}
		}
	}
#endif // _DEBUG


	if(input->TriggerKey(DIK_1))
	{
		// 定数値でウェーブを起こす
		int hitX = 10;
		int hitZ = 10;
		float waveRange = 15.0f;
		float initialYVelocity = 0.86f;
		mapChip_->TriggerWave(hitX, hitZ, waveRange, initialYVelocity);
		
	} /*else if(input->TriggerKey(DIK_2))
	{
		std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
		enemy->Initialize(&enemyModel);
		enemies_.emplace_back(std::move(enemy));
	}*/

	player_->Update();

	for(auto& enemySpawner : enemySpawners_)
	{
		enemySpawner->Update();
		if(!enemySpawner->IsSpawn())
		{
			continue;
		}
			std::unique_ptr<Enemy> enemy;
			enemy.reset(enemySpawner->Spawn());
			enemies_.emplace_back(std::move(enemy));
	}

	for(auto& enemy : enemies_)
	{
		enemy->Update();
	}
	std::erase_if(enemies_,[](std::unique_ptr<Enemy>& enemy){return 1 - enemy->IsAlive();});

	mapChip_->Update();

	object_->UpdateMatrix();

	#pragma region プレイヤーの手が地面に衝突したらカメラのシェイクを起こす

	// プレイヤーの手が地面にめり込んだらシェイク開始
	if (player_->GetHandTranslate().y <= 0.0f) {
		camera->ApplyShake(1.5f, 150);
	}
	// カメラのシェイクを更新
	camera->UpdateShake();

	#pragma endregion

#ifdef _DEBUG // デバッグカメラ
	DebugCameraUpdate(input);
#endif

	CheckAllCollisions();
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

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	/// 

	// オブジェクトの描画
	object_->Draw();
	for(auto& enemySpawner : enemySpawners_)
	{
		enemySpawner->Draw();
	}

	for(auto& enemy : enemies_)
	{
		enemy->Draw();
	}

	player_->Draw();

#pragma region マップチップ描画用PSOに変更->マップチップ描画->通常PSOに戻す
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineStateMapchip());
	// マップチップ
	mapChip_->Draw();
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineState());
#pragma endregion


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

	ImGui::Begin("window");

	ImGui::DragFloat3("Camera translation",&camera->transform.translate.x,0.1f);
	ImGui::DragFloat3("Camera rotate",&camera->transform.rotate.x,0.1f);
	ImGui::DragFloat3("translation",&object_->transform_.translate.x,0.01f);
	ImGui::DragFloat3("rotation",&object_->transform_.rotate.x,0.01f);

#ifdef _DEBUG // デバッグカメラ
	ImGui::Checkbox("useDebugCamera",&useDebugCamera);
#endif

	ImGui::DragFloat3("camera.rotation",&camera->transform.rotate.x,0.01f);

	ImGui::Text("fps : %.1f", ImGui::GetIO().Framerate);

	ImGui::End();

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

void GamePlayScene::GenerateBloks()
{
	// 要素数
	uint32_t kNumBlockVirtical = mapChip_->GetNumBlockVertical();
	uint32_t kNumBlockHorizontal = mapChip_->GetNumBlockHorizontal();

	// ブロックの生成
	objectBlocks_.resize(kNumBlockVirtical); // 垂直方向のサイズを設定
	for(uint32_t i = 0; i < kNumBlockVirtical; ++i)
	{
		objectBlocks_[i].resize(kNumBlockHorizontal); // 水平方向のサイズを設定
		for(uint32_t j = 0; j < kNumBlockHorizontal; ++j)
		{
// ユニークポインタでObject3Dを初期化
			objectBlocks_[i][j] = std::make_unique<Object3D>();

			// モデルの設定
			objectBlocks_[i][j]->model_ = &modelBlock_;

			// 位置の設定
			objectBlocks_[i][j]->transform_.translate = mapChip_->GetMapChipPositionByIndex(j,i);
		}
	}
}

void GamePlayScene::CheckAllCollisions()
{
	for(auto& enemy : enemies_)
	{
		mapChip_->CheckCollision_Collider(enemy->GetCollider());
	}

	mapChip_->CheckCollision_Collider(player_->GetHandCollider());

}
