#include "SceneFactory.h"
#include "Scenes/MainMenu.h"
#include "Scenes/SouthPool.h"

namespace AvatarGame
{
SceneFactory::SceneFactory()
{
    AddScene("MainMenu", [](){return std::make_unique<MainMenu>();});
    AddScene("SouthPool", [](){return std::make_unique<SouthPool>();});
}
}  // namespace AvatarGame
