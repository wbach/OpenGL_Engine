#include "NetworkCharacter.h"
//#include "Common/Hero/Stats.h"
#include "Common/Controllers/ControllersFactory.h"
#include "Common/Controllers/IController.h"

namespace MmmoRpg
{
NetworkCharacter::NetworkCharacter(uint32 id, const common::Hero::CommonStats& stats,
                                   GameEngine::GameObject& gameObject)
    : id_(id)
    , stats_(stats)
    , gameObject_(gameObject)
{
    auto characterController = common::Controllers::CreateCharacterController(gameObject_.GetTransform(), stats_);
    controllers_.insert({ common::Controllers::Types::CharacterControllerType, characterController });
}

std::shared_ptr<common::Controllers::IController> NetworkCharacter::GetControllerByType(common::Controllers::Types type)
{
    if (controllers_.count(type) == 0)
        return nullptr;

    return controllers_.at(type);
}
void NetworkCharacter::UpdateControllers(float dt)
{
    for (auto& controller : controllers_)
        controller.second->Update(dt);
}
uint32 NetworkCharacter::GetId()
{
    return id_;
}
GameEngine::GameObject& NetworkCharacter::GetGameObject()
{
    return gameObject_;
}
}  // namespace MmmoRpg
