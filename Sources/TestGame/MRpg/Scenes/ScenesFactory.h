#pragma once
#include "../MrpgGameContext.h"
#include "Scene/SceneFactoryBase.h"

namespace Network
{
class Gateway;
}  // namespace Network

namespace MmmoRpg
{
class SceneFactory : public GameEngine::SceneFactoryBase
{
   public:
    SceneFactory(Network::Gateway& gateway, std::string& serverAdress, MrpgGameContext& gameContext);

   private:
    GameEngine::ScenePtr CreateMainScene();
    GameEngine::ScenePtr CreateLoginScene();
    GameEngine::ScenePtr CreateSelectCharacterScene();
    GameEngine::ScenePtr CreateGuiEditorScene();

   private:
    Network::Gateway& gateway_;
    std::string& serverAdress_;
    MrpgGameContext& gameContext_;
};
}  // namespace MmmoRpg
