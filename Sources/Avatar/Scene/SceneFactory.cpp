#include "SceneFactory.h"

#include "Scenes/Gorzow.h"
#include "Scenes/GreenIsland.h"
#include "Scenes/Kingdom.h"
#include "Scenes/MainMenu.h"
#include "Scenes/SouthPole.h"

namespace AvatarGame
{
SceneFactory::SceneFactory()
{
    AddScene("MainMenu", GameEngine::File{"Scenes/MainMenu/MainMenu.xml"});
    AddScene("Gorzow", GameEngine::File{"Scenes/Gorzow/Gorzow.xml"});
    AddScene("GreenIsland", GameEngine::File{"Scenes/GreenIsland/GreenIsland.xml"});
    AddScene("SouthPole", GameEngine::File{"Scenes/SouthPole/SouthPole.xml"});
    AddScene("Kingdom", GameEngine::File{"Scenes/Kingdom/Kingdom.xml"});
}
}  // namespace AvatarGame
