#include "Stage.h"

#include "Camera.h"
#include "DirectXBase.h"
#include "ImguiWrapper.h"
#include "Application/DeltaTime/DeltaTime.h"

void Stage::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	GlobalVariables* variables = GlobalVariables::getInstance();

	// SpriteCommonの生成と初期化
	spriteCommon = std::make_unique<SpriteCommon>();
	spriteCommon->Initialize(DirectXBase::GetInstance());

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
	mapChip_->SetStage(this);

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

	// hand
	sideHandModel_[0]  = ModelManager::LoadModelFile("resources/Models","bighand.obj",dxBase->GetDevice());
	sideHandModel_[0].material.textureHandle = TextureManager::Load("resources/Images/bighand.png",dxBase->GetDevice());
	sideHandModel_[1] = ModelManager::LoadModelFile("resources/Models","bighand_left.obj",dxBase->GetDevice());
	sideHandModel_[1].material.textureHandle = TextureManager::Load("resources/Images/bighand.png",dxBase->GetDevice());

	sideHandObject_[0] = std::make_unique<Object3D>();
	sideHandObject_[1] = std::make_unique<Object3D>();
	sideHandObject_[0]->model_ = &sideHandModel_[0];
	sideHandObject_[1]->model_ = &sideHandModel_[1];

	// timer
	timerModel_       = ModelManager::LoadModelFile("resources/Models","timer.obj",dxBase->GetDevice());
	timerModel_.material.textureHandle = TextureManager::Load("resources/Images/timer.png",dxBase->GetDevice());

	timerNeedleModel_ = ModelManager::LoadModelFile("resources/Models","timer_needle.obj",dxBase->GetDevice());
	timerNeedleModel_.material.textureHandle = TextureManager::Load("resources/Images/timer_needle.png",dxBase->GetDevice());

	timerObject_        = std::make_unique<Object3D>();
	timerObject_->model_ = &timerModel_;
	timerNeedleObject_  = std::make_unique<Object3D>();
	timerNeedleObject_->SetParent(timerObject_.get());
	timerNeedleObject_->model_ = &timerNeedleModel_;

	variables->addValue("Game","Timer","scale",timerObject_->transform_.scale);
	variables->addValue("Game","Timer","rotate",timerObject_->transform_.rotate);
	variables->addValue("Game","Timer","translate",timerObject_->transform_.translate);
	variables->addValue("Game","Timer","needle_scale",timerNeedleObject_->transform_.scale);
	variables->addValue("Game","Timer","needle_rotate",timerNeedleObject_->transform_.rotate);
	variables->addValue("Game","Timer","needle_translate",timerNeedleObject_->transform_.translate);

	variables->addValue("Game","Hand","LeftHand_scale",sideHandObject_[0]->transform_.scale);
	variables->addValue("Game","Hand","LeftHand_rotate",sideHandObject_[0]->transform_.rotate);
	variables->addValue("Game","Hand","LeftHand_Translate",sideHandObject_[0]->transform_.translate);
	variables->addValue("Game","Hand","RightHand_scale",sideHandObject_[1]->transform_.scale);
	variables->addValue("Game","Hand","RightHand_rotate",sideHandObject_[1]->transform_.rotate);
	variables->addValue("Game","Hand","RightHand_Translate",sideHandObject_[1]->transform_.translate);

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

	// 背景の星パーティクル関連初期化
	uint32_t backGroundStarParticleGH = TextureManager::Load("resources/Images/backGroundStar.png", dxBase->GetDevice());
	backGroundStarEmitter_.Initialize(backGroundStarParticleGH, spriteCommon.get());

	// 背景スプライト生成
	uint32_t backGroundGH = TextureManager::Load("resources/Images/backGround.png", dxBase->GetDevice());
	backGroundSprite_ = std::make_unique<Sprite>();
	backGroundSprite_->Initialize(spriteCommon.get(), backGroundGH);
	backGroundSprite_->SetSize({ static_cast<float>(Window::GetWidth()), static_cast<float>(Window::GetHeight()) });

	// 背景の雲スプライト生成
	uint32_t nearCloudGH = TextureManager::Load("resources/Images/nearCloud.png", dxBase->GetDevice());
	cloudSprite_ = std::make_unique<Sprite[]>(4);
	cloudSprite_[0].Initialize(spriteCommon.get(), nearCloudGH);
	cloudSprite_[0].SetPosition({0.0f, 0.0f});

	cloudSprite_[1].Initialize(spriteCommon.get(), nearCloudGH);
	cloudSprite_[1].SetPosition({1280.0f, 0.0f});

	uint32_t farCloudGH = TextureManager::Load("resources/Images/farCloud.png", dxBase->GetDevice());
	cloudSprite_[2].Initialize(spriteCommon.get(), farCloudGH);
	cloudSprite_[2].SetPosition({0.0f, 0.0f});
	cloudSprite_[2].SetColor({1.0f, 1.0f, 1.0f, 0.75f});

	cloudSprite_[3].Initialize(spriteCommon.get(), farCloudGH);
	cloudSprite_[3].SetPosition({1280.0f, 0.0f});
	cloudSprite_[3].SetColor({1.0f, 1.0f, 1.0f, 0.75f});


	variables->addValue("Game","Stage","limitTime",limitTime_);
	currentTime_ = limitTime_;

	chargedEnergy_ = 0.0f;

	isClear_ = false;
	isGameOver_ = false;
}

void Stage::Update(Camera* camera)
{
	currentTime_ -= DeltaTime::getInstance()->getDeltaTime();
	if(chargedEnergy_ >= maxEnergy_){
		isClear_ = true;
		return;
	}
	if(currentTime_ < 0.0f)
	{
		 isGameOver_ = true;
		 return;
	}

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

	CheckAlCollisions();

#pragma region パーティクルの発生と更新
	/*--------------------------*/
	/* プレイヤー移動時パーティクル */
	/*--------------------------*/
	
	// プレイヤー移動時にパーティクルを発生させる
	if (player_->IsMoving()) {
		playerMoveEmitter_.Emit(player_->GetTranslate());
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
	if(player_->GetTranslate().y <= 0.0f)
	{
		camera->ApplyShake(0.5f, 120);
	}
	// カメラのシェイクを更新
	camera->UpdateShake();

#pragma endregion
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

	for(size_t i = 0; i < 2; i++)
	{
		sideHandObject_[i]->UpdateMatrix();
		sideHandObject_[i]->Draw();
	}

	timerObject_->UpdateMatrix();
	timerNeedleObject_->UpdateMatrix();
	timerObject_->Draw();
	timerNeedleObject_->Draw();
}

void Stage::UpdateBackGround() {
	// 背景スプライト更新
	backGroundSprite_->Update();

	/*--------------------------*/
	/*  背景の星パーティクル(2D)   */
	/*--------------------------*/

	// 背景の星パーティクルの生成と更新
	backGroundStarEmitter_.Update();

	/*--------------------------*/
	/*        背景の雲           */
	/*--------------------------*/

	/* 手前の雲の更新 */

	UpdateCloudPosition(cloudSprite_[0], near0x, nearCloudMoveSpeed, -1280.0f, 0.0f);
	UpdateCloudPosition(cloudSprite_[1], near1x, nearCloudMoveSpeed, 0.0f, 1280.0f);

	/* 遠くの雲の更新 */

	UpdateCloudPosition(cloudSprite_[2], far2x, farCloudMoveSpeed, -1280.0f, 0.0f);
	UpdateCloudPosition(cloudSprite_[3], far3x, farCloudMoveSpeed, 0.0f, 1280.0f);
}

void Stage::DrawBackGround() { 
	// 背景スプライト描画
	backGroundSprite_->Draw();

	/* パーティクル関連描画 */

	/*--------------------------*/
	/*  背景の星パーティクル(2D)   */
	/*--------------------------*/

	backGroundStarEmitter_.Draw();

	/*--------------------------*/
	/*        背景の雲           */
	/*--------------------------*/

	// 遠くの雲を描画
	cloudSprite_[2].Draw();
	cloudSprite_[3].Draw();

	// 手前の雲を描画
	cloudSprite_[0].Draw();
	cloudSprite_[1].Draw();
}

void Stage::UpdatePlayerAndMapChip(Camera* camera)
{
	if(chargedEnergy_ >= maxEnergy_)
	{
		isClear_ = true;
		return;
	}
	player_->Update();
	mapChip_->Update();

	// Collision
	mapChip_->CheckCollision_Collider(player_->GetHandCollider());

#pragma region パーティクルの発生と更新
	/*--------------------------*/
	/* プレイヤー移動時パーティクル */
	/*--------------------------*/

	// プレイヤー移動時にパーティクルを発生させる
	if(player_->IsMoving())
	{
		playerMoveEmitter_.Emit(player_->GetTranslate());
	}

	// プレイヤー移動時パーティクルを更新
	playerMoveEmitter_.Update();

	// 敵着地時のパーティクルを更新
	enemyLandingEmitter_.Update();

	// 敵分裂時のパーティクルを更新
	enemyDivideEmitter_.Update();
	
#pragma endregion

#pragma region プレイヤーの手が地面に衝突したらカメラのシェイクを起こす

	// プレイヤーの手が地面にめり込んだらシェイク開始
	if(player_->GetTranslate().y <= 0.0f)
	{
		camera->ApplyShake(0.5f,120);
	}
	// カメラのシェイクを更新
	camera->UpdateShake();

#pragma endregion
}

void Stage::InitializeStatus(const std::string& scene)
{
	GlobalVariables* variables = GlobalVariables::getInstance();

	mapChip_->Initialize(modelBlock_);

	if(scene == "Title")
	{
		player_->InitializeStatus();
		player_->SetMapChipField(mapChip_.get());
		player_->SetStage(this);
		variables->addValue("Game","EnemySpawner_Default","spawnerValue",enemySpawnerValue_);
		enemies_.clear();
		enemySpawners_.clear();
		for(size_t i = 0; i < enemySpawnerValue_; ++i)
		{
			enemySpawners_.push_back(std::make_unique<EnemySpawner>());
			enemySpawners_.back()->Initialize(static_cast<int32_t>(enemySpawners_.size() - 1),&enemyModel);
			enemySpawners_.back()->SetEnemyModel(enemyModel);
		}
	}
	
	collisionManager_->Reset();
	variables->addValue(scene,"Stage","maxEnergy",maxEnergy_);

	variables->addValue(scene,"Stage","limitTime",limitTime_);
	currentTime_ = limitTime_;

	chargedEnergy_ = 0.0f;

	isClear_ = false;
	isGameOver_ = false;
}

void Stage::CheckAlCollisions()
{
	for(auto& enemy : enemies_)
	{
		mapChip_->CheckCollision_Collider(enemy->GetCollider());
	}

	mapChip_->CheckCollision_Collider(player_->GetHandCollider());
}

void Stage::ClearEnemies()
{
	enemies_.clear();
	enemyLandingEmitter_.ClearParticles();
}

void Stage::Debug() {
	ImGui::Begin("stage");

	if (ImGui::Button("emit")) {
		backGroundStarEmitter_.Emit();
	}

	ImGui::End();
}

void Stage::UpdateCloudPosition(Sprite& sprite, float& x, float moveSpeed, float resetThreshold, float resetPosition) {
	x -= moveSpeed; 
	if (x < resetThreshold) {
		x = resetPosition;
	}

	sprite.SetPosition({x, 0.0f});
	sprite.Update();
}
