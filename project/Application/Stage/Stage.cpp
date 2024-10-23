#include "Stage.h"

#include <numbers>

#include "Application/DeltaTime/DeltaTime.h"
#include "Camera.h"
#include "DirectXBase.h"
#include "ImguiWrapper.h"

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

	uint32_t enmeyTexture = TextureManager::Load("resources/Images/enemy.png",dxBase->GetDevice());
	variables->addValue("Game","EnemySpawner_Default","spawnerValue",enemySpawnerValue_);
	enemyModel = ModelManager::LoadModelFile("resources/Models","enemy.obj",dxBase->GetDevice());
	enemyModel.material.textureHandle = enmeyTexture;

	for(size_t i = 0; i < enemySpawnerValue_; ++i)
	{
		enemySpawners_.push_back(std::make_unique<EnemySpawner>(this));
		enemySpawners_.back()->Initialize("Game",static_cast<int32_t>(enemySpawners_.size() - 1),&enemyModel);
		enemySpawners_.back()->SetEnemyModel(enemyModel);
	}

	// hand
	sideHandModel_[0] = ModelManager::LoadModelFile("resources/Models","bighand.obj",dxBase->GetDevice());
	sideHandModel_[0].material.textureHandle = TextureManager::Load("resources/Images/bighand.png",dxBase->GetDevice());
	sideHandModel_[1] = ModelManager::LoadModelFile("resources/Models","bighand_left.obj",dxBase->GetDevice());
	sideHandModel_[1].material.textureHandle = TextureManager::Load("resources/Images/bighand.png",dxBase->GetDevice());

	sideHandObject_[0] = std::make_unique<Object3D>();
	sideHandObject_[1] = std::make_unique<Object3D>();
	sideHandObject_[0]->model_ = &sideHandModel_[0];
	sideHandObject_[1]->model_ = &sideHandModel_[1];

	// timer
	timerModel_ = ModelManager::LoadModelFile("resources/Models","timer2.obj",dxBase->GetDevice());
	timerModel_.material.textureHandle = TextureManager::Load("resources/Images/timer2.png",dxBase->GetDevice());

	timerNeedleModel_ = ModelManager::LoadModelFile("resources/Models","timer_needle.obj",dxBase->GetDevice());
	timerNeedleModel_.material.textureHandle = TextureManager::Load("resources/Images/timer_needle.png",dxBase->GetDevice());

	timerObject_ = std::make_unique<Object3D>();
	timerObject_->model_ = &timerModel_;
	//timerObject_->transform_.rotate.x = { 0.01f };
	timerNeedleObject_ = std::make_unique<Object3D>();
	timerNeedleObject_->SetParent(timerObject_.get());
	timerNeedleObject_->model_ = &timerNeedleModel_;
	
	variables->addValue("Game","Timer","scale",timerObject_->transform_.scale);
	variables->addValue("Game","Timer","rotate",timerObject_->transform_.rotate);
	variables->addValue("Game","Timer","translate",timerObject_->transform_.translate);
	variables->addValue("Game","Timer","needle_scale",timerNeedleObject_->transform_.scale);
	variables->addValue("Game","Timer","needle_rotate",timerNeedleStartRotate_);
	variables->addValue("Game","Timer","needle_translate",timerNeedleObject_->transform_.translate);
	timerNeedleObject_->transform_.rotate = timerNeedleStartRotate_;

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
	modelEnemyLandingParticle_ = ModelManager::LoadModelFile("resources/Models","star.obj",dxBase->GetDevice());
	uint32_t enemyLandingParticleGH = TextureManager::Load("resources/Images/star.png",dxBase->GetDevice());

	enemyLandingEmitter_.Initialize(&modelEnemyLandingParticle_,enemyLandingParticleGH);

	// プレイヤー移動時パーティクル関連初期化
	modelPlayerMoveParticle_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	uint32_t playerMoveParticleGH = TextureManager::Load("resources/Images/playerMove.png", dxBase->GetDevice());

	playerMoveEmitter_.Initialize(&modelPlayerMoveParticle_,playerMoveParticleGH);

	// エネミー分裂時パーティクル関連初期化
	modelEnemyDivideParticle_ = ModelManager::LoadModelFile("resources/Models", "sphere.obj", dxBase->GetDevice());
	uint32_t enemyDivideParticleGH = TextureManager::Load("resources/Images/enemyDivide.png", dxBase->GetDevice());

	enemyDivideEmitter_.Initialize(&modelEnemyDivideParticle_,enemyDivideParticleGH);

	// エネミー死亡時パーティクル関連初期化
	modelEnemyDeadParticle_ = ModelManager::LoadModelFile("resources/Models", "enemyDead.obj", dxBase->GetDevice());
	uint32_t enemyDeadParticleGH = TextureManager::Load("resources/Images/enemyDead.png", dxBase->GetDevice());

	enemyDeadEmitter_.Initialize(&modelEnemyDeadParticle_, enemyDeadParticleGH);

	// 背景の星パーティクル関連初期化
	uint32_t backGroundStarParticleGH = TextureManager::Load("resources/Images/backGroundStar.png",dxBase->GetDevice());
	backGroundStarEmitter_.Initialize(backGroundStarParticleGH,spriteCommon.get());

	// 背景スプライト生成
	uint32_t backGroundGH = TextureManager::Load("resources/Images/backGround.png",dxBase->GetDevice());
	backGroundSprite_ = std::make_unique<Sprite>();
	backGroundSprite_->Initialize(spriteCommon.get(),backGroundGH);
	backGroundSprite_->SetSize({static_cast<float>(Window::GetWidth()),static_cast<float>(Window::GetHeight())});

	// 背景の雲スプライト生成
	uint32_t nearCloudGH = TextureManager::Load("resources/Images/nearCloud.png",dxBase->GetDevice());
	cloudSprite_ = std::make_unique<Sprite[]>(4);
	cloudSprite_[0].Initialize(spriteCommon.get(),nearCloudGH);
	cloudSprite_[0].SetPosition({0.0f,0.0f});

	cloudSprite_[1].Initialize(spriteCommon.get(),nearCloudGH);
	cloudSprite_[1].SetPosition({1280.0f,0.0f});

	uint32_t farCloudGH = TextureManager::Load("resources/Images/farCloud.png",dxBase->GetDevice());
	cloudSprite_[2].Initialize(spriteCommon.get(),farCloudGH);
	cloudSprite_[2].SetPosition({0.0f,0.0f});
	cloudSprite_[2].SetColor({1.0f,1.0f,1.0f,0.75f});

	cloudSprite_[3].Initialize(spriteCommon.get(),farCloudGH);
	cloudSprite_[3].SetPosition({1280.0f,0.0f});
	cloudSprite_[3].SetColor({1.0f,1.0f,1.0f,0.75f});


	// モデルの読み込みは一度だけ実行
	for (int i = 0; i < 10; ++i) {
		std::string modelPath = std::to_string(i) + ".obj";
		numberModel_[i] = ModelManager::LoadModelFile("resources/Models/number/", modelPath, dxBase->GetDevice());

		// テクスチャを設定
		numberModel_[i].material.textureHandle = TextureManager::Load("resources/Images/enempng.png", dxBase->GetDevice());
	}

	
	// オブジェクト生成
	for (int j = 0; j < 3; ++j) {
		for (int i = 0; i < 10; ++i) {
			// Object3Dオブジェクトを作成
			numberObject_[i][j] = std::make_unique<Object3D>();
			numberObject_[i][j]->model_ = &numberModel_[i];

			// スケールと位置を設定
			numberObject_[i][j]->transform_.scale = { 6, 6, 2 };
			numberObject_[i][j]->transform_.rotate = { 0, 3.14f, 0 };
			numberObject_[i][j]->transform_.translate = { static_cast<float>(j) * -2.0f - 4.5f, 2, 16 };  // X方向にずらして配置
		}
	}
	
	// パーセント
	percentModel_ = ModelManager::LoadModelFile("resources/Models/", "percent.obj", dxBase->GetDevice());
	percentModel_.material.textureHandle = TextureManager::Load("resources/Images/enempng.png", dxBase->GetDevice());


	percentObject_ = std::make_unique<Object3D>();
	percentObject_->model_ = &percentModel_;
	percentObject_->transform_.scale = { 1.5f, 1.5f, 4 };
	percentObject_->transform_.rotate = { 0, 3.14f, 0 };
	percentObject_->transform_.translate= { -2.1f, 3.5f, 16 };  // X方向にずらして配置
	// オブジェクト生成
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < 10; ++i) {
			// Object3Dオブジェクトを作成
			timerNumberObject_[i][j] = std::make_unique<Object3D>();
			timerNumberObject_[i][j]->model_ = &numberModel_[i];

			// スケールと位置を設定
			timerNumberObject_[i][j]->transform_.scale = { 5, 5, 2 };
			timerNumberObject_[i][j]->transform_.rotate = { -0.6f, 3.14f, 0 };
			timerNumberObject_[i][j]->transform_.translate = { static_cast<float>(j) * -2.0f + 47.1f, 4.7f, 17.1f };  // X方向にずらして配置
		}
	}

	variables->addValue("Game","Stage","limitTime",limitTime_);
	leftTime_ = limitTime_;

	chargedEnergy_ = 0.0f;

	isClear_ = false;
	isGameOver_ = false;
}

void Stage::Update(Camera* camera)
{
	leftTime_ -= DeltaTime::getInstance()->getDeltaTime();
	if(chargedEnergy_ >= maxEnergy_)
	{
		isClear_ = true;
		return;
	}
	timerNeedleObject_->transform_.rotate.z = Lerp(leftTime_ / limitTime_,timerNeedleStartRotate_.z,std::numbers::pi_v<float>*2.0f);
	timerNeedleObject_->UpdateMatrix();
	if(leftTime_ < 0.0f)
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
			enemySpawners_.push_back(std::make_unique<EnemySpawner>(this));
			enemySpawners_.back()->Initialize("Game",static_cast<int32_t>(enemySpawners_.size() - 1),&enemyModel);
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

#ifdef _DEBUG
	if(isSpawnerActive_)
	{
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
	}
#else
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
#endif // _DEBUG

	for(auto& enemy : enemies_)
	{
		enemy->Update(enemies_);
		
		///
		///	敵死亡時パーティクルの発生（敵がelaseされる前に発生させたいのでここで記述）
		/// 
		if (!enemy->IsAlive()) {
			enemyDeadEmitter_.Emit(enemy->GetTranslate());
		}
	}
	std::erase_if(enemies_,[](std::unique_ptr<Enemy>& enemy) { return enemy->IsAlive() ? false : true; });

	mapChip_->Update();

	CheckAlCollisions();

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

	/*--------------------------*/
	/*     敵着地パーティクル      */
	/*--------------------------*/

//	// 敵着地時にパーティクルを発生させる
//	for(auto& enemy : enemies_)
//	{
//		if(enemy->GetLanding())
//		{
//// ウェーブ中のブロックと衝突して大量にパーティクルが出てしまうのをゴリ押しで防ぐ
//			if(enemy->GetTranslate().y <= 2.0f && enemy->GetTranslate().y >= 1.0f)
//			{
//				enemyLandingEmitter_.Emit(enemy->GetTranslate());
//			}
//		}
//	}
//
//	// 敵着地時のパーティクルを更新
//	enemyLandingEmitter_.Update();

	/*--------------------------*/
	/*     敵分裂パーティクル      */
	/*--------------------------*/

	// 敵分裂時にパーティクルを発生させる
	for(auto& enemy : enemies_)
	{
		if(enemy->GetIsCloneThisFrame())
		{
			enemyDivideEmitter_.Emit(enemy->GetTranslate());
			// パーティクル発生後にフラグを下ろす
			enemy->SetIsCloneThisFrame(false);
		}
	}

	// 敵分裂時のパーティクルを更新
	enemyDivideEmitter_.Update();

	/*--------------------------*/
	/*     敵死亡パーティクル      */
	/*--------------------------*/

	// 敵死亡時のパーティクルを更新
	enemyDeadEmitter_.Update();

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

void Stage::DrawModels()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	// オブジェクトの描画
#ifdef _DEBUG
	for(auto& enemySpawner : enemySpawners_)
	{
		enemySpawner->Draw();
	}
#endif // _DEBUG

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

	// 敵死亡時パーティクルを描画
	enemyDeadEmitter_.Draw();

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

	// 描画処理
	// 
	// chargedEnergy_の桁数を取得
	int numDigits = (chargedEnergy_ == 0) ? 1 : static_cast<int>(log10(chargedEnergy_)) + 1;


	for (int j = 0; j < numDigits; ++j) {
		// j桁目の数字を取り出す（右から左へ）
		int digit = (static_cast<int>(chargedEnergy_) / static_cast<int>(pow(10, j))) % 10;

		numberObject_[digit][j]->transform_.translate.x = static_cast<float>(j) * -2.0f - 5.5f;
		// 桁に対応する数字を描画
		numberObject_[digit][j]->UpdateMatrix();
		numberObject_[digit][j]->Draw();
	}
	
	percentObject_->transform_.translate = { -2.5f + (-0.1f*numDigits), 3.5f, 16 };
	percentObject_->UpdateMatrix();
	percentObject_->Draw();

	// 描画処理
	for (int j = 0; j < 2; ++j) {
		// j桁目の数字を取り出す（右から左へ）
		int digit = (static_cast<int>(leftTime_) / static_cast<int>(pow(10, j))) % 10;

		// 桁に対応する数字を描画
		timerNumberObject_[digit][j]->UpdateMatrix();
		timerNumberObject_[digit][j]->Draw();
	}

	
}

void Stage::UpdateBackGround()
{
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

	UpdateCloudPosition(cloudSprite_[0],near0x,nearCloudMoveSpeed,-1280.0f,0.0f);
	UpdateCloudPosition(cloudSprite_[1],near1x,nearCloudMoveSpeed,0.0f,1280.0f);

	/* 遠くの雲の更新 */

	UpdateCloudPosition(cloudSprite_[2],far2x,farCloudMoveSpeed,-1280.0f,0.0f);
	UpdateCloudPosition(cloudSprite_[3],far3x,farCloudMoveSpeed,0.0f,1280.0f);
}

void Stage::DrawBackGround()
{
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
	} else
	{
		isClear_ = false;
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

	/*--------------------------*/
	/*     敵分裂パーティクル      */
	/*--------------------------*/

	// 敵分裂時にパーティクルを発生させる
	for (auto& enemy : enemies_)
	{
		if (enemy->GetIsCloneThisFrame())
		{
			enemyDivideEmitter_.Emit(enemy->GetTranslate());
			// パーティクル発生後にフラグを下ろす
			enemy->SetIsCloneThisFrame(false);
		}
	}

	// 敵死亡時のパーティクルを更新
	enemyDeadEmitter_.Update();

	// プレイヤー移動時パーティクルを更新
	playerMoveEmitter_.Update();

	// 敵着地時のパーティクルを更新
	enemyLandingEmitter_.Update();

	// 敵分裂時のパーティクルを更新
	enemyDivideEmitter_.Update();

	// 敵死亡時のパーティクルを更新
	enemyDeadEmitter_.Update();

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

void Stage::UpdateEnemies()
{
#ifdef _DEBUG
	int32_t movingSpawnerValue = enemySpawnerValue_ - static_cast<int32_t>(enemySpawners_.size());

	if(movingSpawnerValue > 0)  // 増加する場合
	{
		for(int32_t i = 0; i < movingSpawnerValue; ++i)
		{
			enemySpawners_.push_back(std::make_unique<EnemySpawner>(this));
			enemySpawners_.back()->Initialize("Tutorial",static_cast<int32_t>(enemySpawners_.size() - 1),&enemyModel);
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

#ifdef _DEBUG
	if(isSpawnerActive_)
	{
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
	}
#else
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
#endif // _DEBUG

	// Update
	for(auto& enemy : enemies_)
	{
		enemy->Update(enemies_);

		///
		///	敵死亡時パーティクルの発生（敵がelaseされる前に発生させたいのでここで記述）
		/// 
		if (!enemy->IsAlive()) {
			enemyDeadEmitter_.Emit(enemy->GetTranslate());
		}
	}
	// Collision
	for(auto& enemy : enemies_)
	{
		mapChip_->CheckCollision_Collider(enemy->GetCollider());
	}
	std::erase_if(enemies_,[](std::unique_ptr<Enemy>& enemy) { return enemy->IsAlive()?false:true; });

	// 敵着地時のパーティクルを更新
	enemyLandingEmitter_.Update();
}

void Stage::InitializeStatus(const std::string& scene)
{
	GlobalVariables* variables = GlobalVariables::getInstance();

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
			enemySpawners_.push_back(std::make_unique<EnemySpawner>(this));
			enemySpawners_.back()->Initialize("Game",static_cast<int32_t>(enemySpawners_.size() - 1),&enemyModel);
			enemySpawners_.back()->SetEnemyModel(enemyModel);
		}
	} else if(scene == "Tutorial")
	{
		variables->addValue("Tutorial","EnemySpawner_Default","spawnerValue",enemySpawnerValue_);
		enemies_.clear();
		enemySpawners_.clear();
		for(size_t i = 0; i < enemySpawnerValue_; ++i)
		{
			enemySpawners_.push_back(std::make_unique<EnemySpawner>(this));
			enemySpawners_.back()->Initialize("Tutorial",static_cast<int32_t>(enemySpawners_.size() - 1),&enemyModel);
			enemySpawners_.back()->SetEnemyModel(enemyModel);
		}
	}

	collisionManager_->Reset();
	variables->addValue(scene,"Stage","maxEnergy",maxEnergy_);

	variables->addValue(scene,"Stage","limitTime",limitTime_);
	leftTime_ = limitTime_;

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

void Stage::Debug()
{
#ifdef _DEBUG
	ImGui::Begin("stage");

	if(ImGui::Button("emit"))
	{
		enemyDeadEmitter_.Emit({10.0f, 10.0f, 0.0f});
	}

	ImGui::Begin("EnemySpawners State");
	ImGui::Checkbox("Spawner is Active",&isSpawnerActive_);
	ImGui::End();

	ImGui::End();
#endif // _DEBUG
}

void Stage::UpdateCloudPosition(Sprite& sprite,float& x,float moveSpeed,float resetThreshold,float resetPosition)
{
	x -= moveSpeed;
	if(x < resetThreshold)
	{
		x = resetPosition;
	}

	sprite.SetPosition({x, cloudY});
	sprite.Update();
}

void Stage::UpBackGroundCloud() {
	// 背景の雲を上へ移動させる
	if (cloudY > -720.0f) {
		cloudY -= kCloudUpSpeed;
	} 
}

void Stage::DownBackGroundCloud() {
	// 背景の雲を上へ移動させる
	if (cloudY < 720.0f) {
		cloudY += kCloudUpSpeed;
	}
}

void Stage::RestoreBackGroundCloud() { 
	// 0より小さい場合は0になるまで増加
	if (cloudY < 0.0f) {
		cloudY += kCloudUpSpeed;
	// 0より大きい場合は0になるまで減少
	} else if (cloudY > 0.0f) {
		cloudY -= kCloudUpSpeed;
	}

	if (cloudY == 0.0f) {
		cloudY = 0.0f;
	}
}

