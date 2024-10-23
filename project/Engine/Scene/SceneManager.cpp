#include "SceneManager.h"
#include <cassert>

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;
	return &instance;
}

SceneManager::~SceneManager()
{
	// 最後のシーンの終了と開放
	scene_->Finalize();
	delete scene_;
}

void SceneManager::InitializeSound()
{
	soundManager_ = std::make_unique<SoundManager>();
	soundManager_->Initialize();

	bgm_ = soundManager_->LoadWave("resources/Sounds/BGM.wav");
	soundManager_->PlayWave(bgm_,true,0.4f);

	beforeHand_ = std::make_unique<BeforehandLoadingTexture>();
	beforeHand_->Initialize();
}

void SceneManager::CameraInitialize()
{
	// カメラのインスタンスを生成
	camera_ = std::make_unique<Camera>(Float3{20.0f,40.0f,-60.0f},Float3{0.44f,0.0f,0.0f},0.45f);
	Camera::Set(camera_.get()); // 現在のカメラをセット

	// カメラのoriginalPositionに現在のカメラのtranslateをセット（シェイク時に使用、ずれを防止するために必要）
	camera_->SetOriginalPosition(camera_->transform.translate);
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	// 次シーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

void SceneManager::Update()
{
	///
	///	シーン切り替え機構
	/// 
	
	// 次シーンの予約があるなら
	if (nextScene_) {
		// 旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}

		// シーン切り替え
		scene_ = nextScene_;
		nextScene_->Finalize();
		nextScene_ = nullptr;

		// 次シーンを初期化する
		// Factory で 済
		//scene_->Initialize();
	}

	// 実行中シーンを更新する
	scene_->Update();
}

void SceneManager::Draw()
{
	// 実行中シーンの描画
	scene_->Draw();
}

void SceneManager::CreateStage()
{
	stage_ = std::make_unique<Stage>();
	stage_->Initialize(soundManager_.get());
}
