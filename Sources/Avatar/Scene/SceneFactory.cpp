#include "SceneFactory.h"
#include "Scenes/GreenIsland.h"
#include "Scenes/Kingdom.h"
#include "Scenes/MainMenu.h"
#include "Scenes/SouthPole.h"

namespace AvatarGame
{
SceneFactory::SceneFactory()
{
    AddScene("MainMenu", []() { return std::make_unique<MainMenu>(); });
    AddScene("GreenIsland", []() { return std::make_unique<GreenIsland>(); });
    AddScene("SouthPole", []() { return std::make_unique<SouthPole>(); });
    AddScene("Kingdom", []() { return std::make_unique<Kingdom>(); });
}
}  // namespace AvatarGame
