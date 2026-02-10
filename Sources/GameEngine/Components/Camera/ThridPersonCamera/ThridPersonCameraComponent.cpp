#include "ThridPersonCameraComponent.h"

#include <Input/InputManager.h>
#include <Utils/Fsm/Fsm.h>
#include <Utils/TreeNode.h>
#include <Utils/Variant.h>

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/Fsm/ThridPersonCameraEvents.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "glm/glm.hpp"

namespace GameEngine
{
namespace Components
{
using namespace Camera;
namespace
{
const std::string& COMPONENT_STR{"ThridPersonCameraComponent"};
}

ThridPersonCameraComponent::ThridPersonCameraComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<ThridPersonCameraComponent>(), componentContext, gameObject)
{
}

void ThridPersonCameraComponent::CleanUp()
{
    componentContext_.scene_.GetCameraManager().RemoveCamera(&fsmContext->camera);

    fsm.reset();
    fsmContext.reset();

    if (subscribtionOnTransformSnapshot)
    {
        thisObject_.UnsubscribeOnWorldTransfromSnapsot(*subscribtionOnTransformSnapshot);
    }
}

void ThridPersonCameraComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, [this]() { awake(); });
    RegisterFunction(FunctionType::OnStart, [this]() { init(); });
    RegisterFunction(FunctionType::Update,
                     [this]()
                     {
                         auto mouseMove = componentContext_.inputManager_.CalcualteMouseMove();
                         if (mouseMove.x != 0 or mouseMove.y != 0)
                             fsm->handle(MouseMoveEvent{.move = mouseMove});
                         processEvent();
                     });
}

void ThridPersonCameraComponent::Reload()
{
}

std::tuple<float, float> ThridPersonCameraComponent::getRotation() const
{
    if (fsmContext)
    {
        return std::make_tuple(fsmContext->pitch, fsmContext->yaw);
    }
    return {};
}

vec3 ThridPersonCameraComponent::getDirection() const
{
    vec3 dir(1.f);
    auto yaw   = ToRadians(fsmContext->yaw - 90);
    auto pitch = -ToRadians(fsmContext->pitch);

    auto xzLen = cosf(pitch);
    dir.x      = xzLen * cos(yaw);
    dir.y      = sinf(pitch);
    dir.z      = xzLen * sinf(-yaw);
    return glm::normalize(dir);
}

void ThridPersonCameraComponent::awake()
{
    if (not componentContext_.scene_.getDisplayManager())
    {
        LOG_ERROR << "Display manager not set!";
        return;
    }

    // TO DO: capture existing camera instead creating new one
    auto camera = std::make_unique<CustomCamera>();

    fsmContext.reset(new Context{.inputManager    = componentContext_.inputManager_,
                                 .displayManager  = *componentContext_.scene_.getDisplayManager(),
                                 .gameObject      = thisObject_,
                                 .camera          = *camera,
                                 .cameraPositions = {.run = vec3{-0.5f, 1.0f, -1.5f}, .aim = vec3{-0.25f, 1.f, -0.75f}}});

    auto& cameraManager = componentContext_.scene_.GetCameraManager();
    cameraId            = cameraManager.AddCamera(std::move(camera));
    fsm                 = std::make_unique<ThridPersonCameraFsm>(FollowingState(*fsmContext), RotateableRunState(*fsmContext),
                                                 AimState(*fsmContext), TransitionState(*fsmContext));

    // std::apply([](auto&&... state) {((state.init()), ...);}, fsm->states);
    fsm->handle(InitEvent{});
}

void ThridPersonCameraComponent::init()
{
    if (not fsmContext or not cameraId)
    {
        return;
    }

    auto& cameraManager = componentContext_.scene_.GetCameraManager();
    cameraManager.ActivateCamera(*cameraId);
    cameraManager.SetCameraAsMain(&fsmContext->camera);

    fsm = std::make_unique<ThridPersonCameraFsm>(FollowingState(*fsmContext), RotateableRunState(*fsmContext),
                                                 AimState(*fsmContext), TransitionState(*fsmContext));

    fsm->handle(InitEvent{});
    componentContext_.inputManager_.SetReleativeMouseMode(true);

    subscribtionOnTransformSnapshot = thisObject_.SubscribeOnWorldTransfomSnapshot(
        [&](const auto& transformContext)
        {
            if (fsmContext)
            {
                fsmContext->lookAtTransformContext = transformContext;
                fsmContext->camera.Update();
                //std::apply([](auto&&... state) { ((state.update()), ...); }, fsm->states);
            }
        });
}

void ThridPersonCameraComponent::processEvent()
{
    if (not eventQueue.empty())
    {
        auto tmpEvents = std::move(eventQueue);
        for (auto& event : tmpEvents)
        {
            std::visit([&](const auto& e) { fsm->handle(e); }, event);
        }
    }
}

void ThridPersonCameraComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode&, GameObject& gameObject)
    { return std::make_unique<ThridPersonCameraComponent>(componentContext, gameObject); };

    regsiterComponentReadFunction(GetComponentType<ThridPersonCameraComponent>(), func);
}

void ThridPersonCameraComponent::write(TreeNode& node) const
{
    const std::string CSTR_TYPE = "type";
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}
}  // namespace Components
}  // namespace GameEngine
