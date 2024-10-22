#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

#include "Application/Stage/Stage.h"

#include "SoundManager.h"

// シーン管理
class SceneManager
{
public:
	static SceneManager* GetInstance();

	~SceneManager();

	void InitializeSound();

	void CameraInitialize();

	// 次シーン予約
	/*void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }*/
	void ChangeScene(const std::string& sceneName);

	// 更新
	void Update();

	// 描画
	void Draw();

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:
	// 今のシーン（実行中シーン）
	BaseScene* scene_ = nullptr;
	// 次のシーン
	BaseScene* nextScene_ = nullptr;
	// シーンファクトリー（借りてくる）
	AbstractSceneFactory* sceneFactory_ = nullptr;

	///===========================================================================================
	/// 各シーンで 共有
	///===========================================================================================
	std::unique_ptr<Camera> camera_ = nullptr;
	std::unique_ptr<Stage> stage_;

	std::unique_ptr<SoundManager> soundManager_ = nullptr;
	SoundManager::SoundData bgm_;
public:
	void CreateStage();
	Stage* GetStage(){return stage_.get();}
	Camera* GetCamera(){return camera_.get();}
};

