#include "RecoilState.h"

#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilState::RecoilState(FsmContext& contex)
    : RecoilStateBase(contex)
{
}
void RecoilState::onLeave(const AimStopEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilState::onLeave(const WeaponStateEvent &e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
