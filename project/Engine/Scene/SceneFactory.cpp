#include "SceneFactory.h"
#include "TitleScene.h"
#include "GamePlayScene.h"
#include "GameOverScene.h"
#include "GameClearScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
    // 次のシーンを生成
    BaseScene* newScene = nullptr;

    if (sceneName == "TITLE") {
        newScene = new TitleScene();
        newScene->Initialize();
    } else if (sceneName == "GAMEPLAY") {
        newScene = new GamePlayScene();
        newScene->Initialize();
    } else if(sceneName == "GAMEOVER")
    {
        newScene = new GameOverScene();
        newScene->Initialize();
    } else if(sceneName == "GAMECLEAR")
    {
        newScene = new GameClearScene();
        newScene->Initialize();
    }

    return newScene;
}
