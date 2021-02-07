#include "SceneFactory.h"
#include "Scenes/MainMenu.h"
#include "Scenes/SouthPole.h"

namespace AvatarGame
{
SceneFactory::SceneFactory()
{
    AddScene("MainMenu", [](){return std::make_unique<MainMenu>();});
    AddScene("SouthPole", [](){return std::make_unique<SouthPole>();});
}
}  // namespace AvatarGame
