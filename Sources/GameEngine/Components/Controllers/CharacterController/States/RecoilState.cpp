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
}  // namespace Components
}  // namespace GameEngine
