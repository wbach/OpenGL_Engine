#include "RotateableRunState.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Display/DisplayManager.hpp"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
RotateableRunState::RotateableRunState(Context& context)
    : StateBase(context, context.cameraPositions.run)
{
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
