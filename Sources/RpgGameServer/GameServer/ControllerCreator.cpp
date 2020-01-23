#include "ControllerCreator.h"
#include "Common/Controllers/ControllersFactory.h"
#include "Common/Hero/Stats.h"

namespace GameServer
{
void CreateCharacterController(Hero::BaseHeroPtr hero)
{
    hero->AddController(common::Controllers::CreateCharacterController(hero->context_.transform_, hero->context_.commonStats_));
}
}  // namespace GameServer
