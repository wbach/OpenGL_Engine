#include "AIController.h"

#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>
#include <Utils/Variant.h>

#include <limits>
#include <memory>

#include "AIControllerContext.h"
#include "AIControllerImpl.inl"
#include "Blackboard.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/AI/ActionStateMachine/ActionAIEvents.h"
#include "GameEngine/Components/Controllers/AI/IntentStateMachine/IntentStateEvents.h"
#include "GameEngine/Components/Controllers/AI/ReactionStateMachine/ReactionAIEvents.h"
#include "GameEngine/Components/Controllers/AI/ReactionStateMachine/ReactionStateMachine.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Time/DayNightCycle.h"
#include "ReactionStateMachine/ReactionStateMachine.h"
#include "RoutineComponent.h"
#include "Utils.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{

REGISTER_COMPONENT(AIController)

AIController::AIController(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
    impl = std::make_unique<AIController::Impl>();
}
AIController::~AIController()
{
}
void AIController::CleanUp()
{
}
void AIController::ReqisterFunctions()
{
    RegisterFunction(
        FunctionType::OnStart, [this]() { Init(); }, MakeDependencies<CharacterController>());
    RegisterFunction(FunctionType::Update, [this]() { Update(); });
}
void AIController::Reload()
{
}
void AIController::Init()
{
    auto characterController = thisObject_.GetComponent<CharacterController>();

    if (not characterController)
    {
        LOG_DEBUG << "CharacterController not found";
        return;
    }

    impl->controllerContext_.reset(
        new AIControllerContext{.gameObject          = thisObject_,
                                .navigationManager   = componentContext_.navigationManager,
                                .characterController = *characterController,
                                .controller          = *this,
                                .anchorPosition_     = thisObject_.GetWorldTransform().GetPosition(),
                                .anchorRotation_     = thisObject_.GetWorldTransform().GetRotation().value_});

    LOG_DEBUG << "Create fsm";
    auto& context       = *impl->controllerContext_;
    impl->stateMachine_ = std::make_unique<ActionStateMachine>(AIAmbientState{}, AIReturnState{context}, AIMoveToState{context});

    impl->intentContext_.reset(new IntentContext{.routineComponent    = thisObject_.GetComponent<RoutineComponent>(),
                                                 .gameObject          = thisObject_,
                                                 .controller          = *this,
                                                 .navigationManager   = componentContext_.navigationManager,
                                                 .characterController = *characterController,
                                                 .currentPath         = impl->controllerContext_->currentPath});

    impl->intentStateMachine_ = std::make_unique<IntentStateMachine>(
        RoutineState{impl->intentContext_->routineComponent}, ReactionState{*impl->intentContext_}, QuestState{}, DeadState{});

    LOG_DEBUG << thisObject_.GetName() << " RoutineComponent " << (impl->intentContext_->routineComponent != nullptr);
}
void AIController::read(const TreeNode& input)
{
}

void AIController::write(TreeNode& node) const
{
}
void AIController::Update()
{
    if (not IsActive())
        return;

    if (not impl->stateMachine_)
        return;

    processEvent();

    if (impl->stateMachine_)
    {
        std::visit([&](auto statePtr) { statePtr->update(componentContext_.time_.deltaTime); },
                   impl->stateMachine_->currentState);
    }
}
void AIController::processEvent()
{
    EventQueue tmpEventsQueue;

    int safetyCounter    = 0;
    const int maxBatches = 10;

    while (safetyCounter < maxBatches)
    {
        {
            std::lock_guard<std::mutex> lk(eventQueueMutex);
            if (eventQueue.empty())
            {
                return;
            }
            tmpEventsQueue = std::move(eventQueue);
        }

        for (auto& event : tmpEventsQueue)
        {
            handleEvent(event);
        }

        ++safetyCounter;
    }
}
void AIController::handleEvent(const AIEvent& aiEvent)
{
    std::visit(visitor{[&](const IntentAIEvent& event)
                       {
                           auto passEventToMachine = [&](const auto& e)
                           {
                               LOG_DEBUG << Utils::GetTypeName(e);
                               impl->intentStateMachine_->handle(e);
                           };
                           std::visit(passEventToMachine, event);
                       },
                       [&](const ReactionAIEvent& event) { impl->intentStateMachine_->handle(event); },
                       [&](const CombatStateEvent& event) { impl->intentStateMachine_->handle(event); },
                       [&](const ActionAIEvent& event)
                       {
                           auto passEventToMachine = [&](const auto& e)
                           {
                               LOG_DEBUG << Utils::GetTypeName(e);
                               impl->stateMachine_->handle(e);
                           };
                           std::visit(passEventToMachine, event);
                       }},
               aiEvent);
}

const std::vector<vec3>& AIController::getCurrentPath() const
{
    if (impl and impl->controllerContext_)
    {
        return impl->controllerContext_->currentPath;
    }

    static std::vector<vec3> result;
    return result;
};
float AIController::getMeleAttackRadius() const
{
    return 1.0f;
}
std::optional<float> AIController::getRangeAttackRadius() const
{
    return std::nullopt;
}
const AIController::Impl& AIController::getImpl() const
{
    return *impl;
}
}  // namespace Components
}  // namespace GameEngine
