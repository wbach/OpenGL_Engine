#include "ThridPersonCameraComponent.h"

#include <Input/InputManager.h>

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"

#include <Utils/Fsm/Fsm.h>

namespace GameEngine
{
namespace Components
{
using namespace Camera;
namespace
{
const std::string& COMPONENT_STR{"ThridPersonCamera"};
}
ThridPersonCameraComponent::ThridPersonCameraComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(ThridPersonCameraComponent).hash_code(), componentContext, gameObject)
{
}

void ThridPersonCameraComponent::CleanUp()
{
    DEBUG_LOG("CleanUp()");
    fsm.reset();
    fsmContext.reset();
}

void ThridPersonCameraComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() { init(); });
}

void ThridPersonCameraComponent::init()
{
    DEBUG_LOG("init");
    if (not componentContext_.scene_.getDisplayManager())
    {
        ERROR_LOG("Display manager not set!");
        return;
    }
    auto camera = std::make_unique<CustomCamera>();
    fsmContext.reset(new Context{componentContext_.inputManager_, *componentContext_.scene_.getDisplayManager(), thisObject_, *camera});
    componentContext_.camera_.addAndSet(std::move(camera));

    fsm = std::make_unique<ThridPersonCameraFsm>(RotateableRunState(*fsmContext), AimState(*fsmContext));

    // std::apply([](auto&&... state) {((state.init()), ...);}, fsm->states);
    fsm->handle(InitEvent{});
}

void ThridPersonCameraComponent::registerReadFunctions()
{
    ReadFunctions::instance().componentsReadFunctions.insert(
        {COMPONENT_STR, [](ComponentContext& componentContext, const TreeNode&, GameObject& gameObject) {
             return std::make_unique<ThridPersonCameraComponent>(componentContext, gameObject);
         }});
}

void ThridPersonCameraComponent::write(TreeNode& node) const
{
    const std::string CSTR_TYPE = "type";
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}

}  // namespace Components
}  // namespace GameEngine
