#include "ControllersFactory.h"

#include "../Hero/Stats.h"
#include "../Transform.h"
#include "CharacterController/Character.h"

namespace common
{
namespace Controllers
{
std::shared_ptr<IController> CreateCharacterController(Transform& attachedTranform, const Hero::CommonStats& stats)
{
    return std::make_shared<CharacterController>(attachedTranform, stats.runSpeed, stats.turnSpeed, stats.jumpPower);
}
}  // namespace Controllers
}  // namespace common