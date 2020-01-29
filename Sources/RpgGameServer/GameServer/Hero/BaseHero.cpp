#include "BaseHero.h"

namespace GameServer
{
namespace Hero
{
void BaseHero::UpdateAllControllers(float dt)
{
    for (auto& controller : controllers_)
        controller.second->Update(dt);
}
void BaseHero::AddController(common::Controllers::IControllerPtr cotroller)
{
    controllers_[cotroller->GetType()] = cotroller;
}
common::Controllers::IControllerPtr BaseHero::GetControllerByType(common::Controllers::Types type)
{
    if (controllers_.count(type) == 0)
        return nullptr;

    return controllers_[type];
}
}  // namespace Hero
}  // namespace GameServer
