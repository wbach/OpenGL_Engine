#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"

#include "Editor/Context.h"
#include "GameEngine/Api/Dummy/DummyGraphicsApi.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Physics/Bach/BachPhysicsAdapter.h"
#include "GameEngine/Physics/Bullet/BulletAdapter.h"
#include "Scene/SceneFactory.h"

using namespace GameEngine;
using namespace GameEngine::Physics;

int main(int, char**)
{
    Editor::Context editorContext;

    Engine engine(std::make_unique<Bullet::BulletAdapter>(), std::make_unique<Editor::SceneFactory>(editorContext));
    engine.Init();
    engine.GetSceneManager().SetActiveScene("EditorScene");
    engine.GameLoop();

    return 0;
}
