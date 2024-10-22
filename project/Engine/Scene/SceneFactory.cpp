#include "SceneFactory.h"

#include "GameClearScene.h"
#include "GameOverScene.h"
#include "GamePlayScene.h"
#include "TitleScene.h"
#include "TutorialScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
    // 次のシーンを生成
    BaseScene* newScene = nullptr;

    if (sceneName == "TITLE") {
        newScene = new TitleScene();
    } else if (sceneName == "GAMEPLAY") {
        newScene = new GamePlayScene();
    } else if(sceneName == "GAMEOVER")
    {
        newScene = new GameOverScene();
    } else if(sceneName == "GAMECLEAR")
    {
        newScene = new GameClearScene();
    } else if(sceneName == "TUTORIAL")
    {
        newScene = new TutorialScene();
    }

    newScene->Initialize();

    return newScene;
}
