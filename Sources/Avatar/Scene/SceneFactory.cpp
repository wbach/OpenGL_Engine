#include "SceneFactory.h"
#include "Scenes/GreenIsland.h"
#include "Scenes/Kingdom.h"
#include "Scenes/MainMenu.h"
#include "Scenes/SouthPole.h"
#include "Scenes/Gorzow.h"

namespace AvatarGame
{
SceneFactory::SceneFactory()
{
    AddScene("MainMenu", [this]() { return std::make_unique<MainMenu>(GetAvaiableScenes());});
    AddScene("GreenIsland", []() { return std::make_unique<GreenIsland>(); });
    AddScene("SouthPole", []() { return std::make_unique<SouthPole>(); });
    AddScene("Kingdom", []() { return std::make_unique<Kingdom>(); });
    AddScene("Gorzow", []() { return std::make_unique<Gorzow>(); });
}
}  // namespace AvatarGame
