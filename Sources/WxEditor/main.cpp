#include <GameEngine/Engine/Engine.h>
#include <GameEngine/Physics/Bullet/BulletAdapter.h>
#include <GameEngine/Scene/SceneFactoryBase.h>

#include "App.h"

wxIMPLEMENT_APP_NO_MAIN(App);

using namespace GameEngine;
using namespace GameEngine::Physics;

class SceneFactory : public GameEngine::SceneFactoryBase
{
public:
    SceneFactory(const GameEngine::File& file)
    {
        AddScene(file.GetBaseName(), file);
    }
};

int main(int argc, char* argv[])
{
    if (argc > 1 && std::string(argv[1]) == "--scene")
    {
        if (argc > 2)
        {
            File file{argv[2]};
            if (file.exist())
            {
                std::cout << "Starting game engine with scene: " << file << "\n";
                Engine engine(std::make_unique<Bullet::BulletAdapter>(), std::make_unique<SceneFactory>(file));
                engine.Init();
                engine.GetSceneManager().SetActiveScene(file.GetBaseName());
                engine.GameLoop();
                return 0;
            }
        }
        std::cout << "Starting game engine failed" << "\n";
    }

    return wxEntry(argc, argv);
}
