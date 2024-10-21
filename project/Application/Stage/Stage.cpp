#include "Stage.h"

#include "Camera.h"
#include "DirectXBase.h"
#include "ImguiWrapper.h"
#include "Application/DeltaTime/DeltaTime.h"

void Stage::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	GlobalVariables* variables = GlobalVariables::getInstance();

	// Texture読み込み
	uint32_t uvCheckerGH = TextureManager::Load("resources/Images/block.png",dxBase->GetDevice());

	// モデルの読み込みとテクスチャの設定(マップチップ)
	modelBlock_ = ModelManager::LoadModelFile("resources/Models","block.obj",dxBase->GetDevice());
	modelBlock_.material.textureHandle = uvCheckerGH;

	// マップチップ
	mapChip_ = std::make_unique<MapChipField>();
	// まずCSVファイルで読み込む
	//mapChip_->LoadMapChipCsv("resources/blocks.csv");
	// そのあとに初期化
	mapChip_->Initialize(modelBlock_);

	// Texture読み込み
	uint32_t monsterBallTexture = TextureManager::Load("resources/Images/monsterBall.png",dxBase->GetDevice());
	player_ = std::make_unique<Player>();
	player_->Initialize(monsterBallTexture);
	player_->SetMapChipField(mapChip_.get());
	player_->SetStage(this);

	variables->addValue("Game","EnemySpawner_Default","spawnerValue",enemySpawnerValue_);
	enemyModel = ModelManager::LoadModelFile("resources/Models","enemy.obj",dxBase->GetDevice());
	enemyModel.material.textureHandle = monsterBallTexture;

	for(size_t i = 0; i < enemySpawnerValue_; ++i)
	{
		enemySpawners_.push_back(std::make_unique<EnemySpawner>());
		enemySpawners_.back()->Initialize(static_cast<int32_t>(enemySpawners_.size() - 1),&enemyModel);
		enemySpawners_.back()->SetEnemyModel(enemyModel);
	}

	// 衝突マネージャの生成
	collisionManager_ = std::make_unique<CollisionManager>();

	variables->addValue("Game","Stage","maxEnergy",maxEnergy_);


	/* パーティクル関連 */
	
	// エネミー着地時パーティクル関連初期化
	modelEnemyLandingParticle_ = ModelManager::LoadModelFile("resources/Models", "star.obj", dxBase->GetDevice());
	uint32_t enemyLandingParticleGH = TextureManager::Load("resources/Images/star.png", dxBase->GetDevice());

	enemyLandingEmitter_.Initialize(&modelEnemyLandingParticle_, enemyLandingParticleGH);

	// プレイヤー移動時パーティクル関連初期化
	modelPlayerMoveParticle_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	uint32_t playerMoveParticleGH = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	playerMoveEmitter_.Initialize(&modelPlayerMoveParticle_, playerMoveParticleGH);

	// エネミー分裂時パーティクル関連初期化
	modelEnemyDivideParticle_ = ModelManager::LoadModelFile("resources/Models", "sphere.obj", dxBase->GetDevice());
	uint32_t enemyDivideParticleGH = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	enemyDivideEmitter_.Initialize(&modelEnemyDivideParticle_, enemyDivideParticleGH);

	variables->addValue("Game","Stage","limitTime",limitTime_);
	currentTime_ = limitTime_;

	isClear_ = false;
}

void Stage::Update(Camera* camera)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	/*
	currentTime_ -= DeltaTime::getInstance()->getDeltaTime();
	if(chargedEnergy_ >= maxEnergy_){
		isClear_ = true;
		return;
	}
	if(currentTime_ < 0.0f)
	{
	 gameOver;
	 return;
	}
	*/

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
		enemy->SetStage(this);
		enemies_.emplace_back(std::move(enemy));
	}

	for(auto& enemy : enemies_)
	{
		enemy->Update(enemies_);
	}
	std::erase_if(enemies_,[](std::unique_ptr<Enemy>& enemy) { return enemy->IsAlive() ? false : true; });

	mapChip_->Update();
	mapChip_->SetPower(chargedEnergy_);

	CheckAlCollisions();

#pragma region パーティクルの発生と更新
	/*--------------------------*/
	/* プレイヤー移動時パーティクル */
	/*--------------------------*/
	
	// プレイヤー移動時にパーティクルを発生させる
	if (player_->IsMoving()) {
		playerMoveEmitter_.Emit(
			{
				player_->GetBodyTranslate().x,
				player_->GetBodyTranslate().y - 1.0f,
				player_->GetBodyTranslate().z
			}
		);
	}

	// プレイヤー移動時パーティクルを更新
	playerMoveEmitter_.Update();

	/*--------------------------*/
	/*     敵着地パーティクル      */
	/*--------------------------*/

	// 敵着地時にパーティクルを発生させる
	for (auto& enemy : enemies_) {
		if (enemy->GetLanding()) {
			// ウェーブ中のブロックと衝突して大量にパーティクルが出てしまうのをゴリ押しで防ぐ
			if (enemy->GetTranslate().y <= 2.0f && enemy->GetTranslate().y >= 0.0f) {
				enemyLandingEmitter_.Emit(enemy->GetTranslate());
			}
		}
	}

	// 敵着地時のパーティクルを更新
	enemyLandingEmitter_.Update();

	/*--------------------------*/
	/*     敵分裂パーティクル      */
	/*--------------------------*/

	// 敵分裂時にパーティクルを発生させる
	for (auto& enemy : enemies_) {
		if (enemy->GetIsCloneThisFrame()) {
			enemyDivideEmitter_.Emit(enemy->GetTranslate());
			// パーティクル発生後にフラグを下ろす
			enemy->SetIsCloneThisFrame(false);
		}
	}

	// 敵分裂時のパーティクルを更新
	enemyDivideEmitter_.Update();

#pragma endregion

#pragma region プレイヤーの手が地面に衝突したらカメラのシェイクを起こす

	// プレイヤーの手が地面にめり込んだらシェイク開始
	if(player_->GetHandTranslate().y <= 0.0f)
	{
		camera->ApplyShake(0.5f, 120);
	}
	// カメラのシェイクを更新
	camera->UpdateShake();

#pragma endregion
	//_CrtDumpMemoryLeaks();
}

void Stage::DrawModels()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	// オブジェクトの描画
	for(auto& enemySpawner : enemySpawners_)
	{
		enemySpawner->Draw();
	}

	for(auto& enemy : enemies_)
	{
		enemy->Draw();
	}

	player_->Draw();

	/* パーティクル関連描画 */

	// プレイヤー移動時パーティクルを描画
	playerMoveEmitter_.Draw();

	// 敵落下時パーティクルを描画
	enemyLandingEmitter_.Draw();

	// 敵分裂時パーティクルを描画
	enemyDivideEmitter_.Draw();

#pragma region マップチップ描画用PSOに変更->マップチップ描画->通常PSOに戻す
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineStateMapchip());
	// マップチップ
	mapChip_->Draw();
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineState());
#pragma endregion
}

void Stage::CheckAlCollisions()
{
	for(auto& enemy : enemies_)
	{
		mapChip_->CheckCollision_Collider(enemy->GetCollider());
	}

	mapChip_->CheckCollision_Collider(player_->GetHandCollider());
}

void Stage::Debug() { 
	ImGui::Begin("stage");

	if (ImGui::Button("emit")) {
		enemyDivideEmitter_.Emit({0.0f, 10.0f, 0.0f});
	}

	ImGui::End();
}
