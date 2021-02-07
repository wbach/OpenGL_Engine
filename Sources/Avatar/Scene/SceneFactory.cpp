#include "SceneFactory.h"
#include "Scenes/MainMenu.h"
#include "Scenes/SouthPole.h"
#include "Scenes/GreenIsland.h"

namespace AvatarGame
{
SceneFactory::SceneFactory()
{
    AddScene("MainMenu", [](){return std::make_unique<MainMenu>();});
    AddScene("GreenIsland", []() {return std::make_unique<GreenIsland>(); });
    AddScene("SouthPole", [](){return std::make_unique<SouthPole>();});
}
}  // namespace AvatarGame
