#include "AIController.h"

#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <limits>
#include <memory>

#include "AICharacterFsm.h"
#include "AIControllerContext.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/AI/States/AIAmbientState.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Components/Gameplay/CharacterStats/CharacterStatsComponent.h"
#include "GameEngine/Components/Gameplay/HealthComponent.h"
#include "GameEngine/Narrative/FactionManager.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_DETECTION_RADIUS[] = "detectionRadius";
constexpr char CSTR_TARGETING_MODE[]   = "targetingMode";
}  // namespace
struct AIController::Impl
{
    std::unique_ptr<AICharacterFsm> stateMachine_;
    std::unique_ptr<AIControllerContext> controllerContext_;

    void CleanUp()
    {
        stateMachine_.reset();
    }
};

REGISTER_COMPONENT(AIController)

AIController::AIController(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
    impl = std::make_unique<AIController::Impl>();
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
    myStats = thisObject_.GetComponent<CharacterStatsComponent>();

    impl->controllerContext_.reset(new AIControllerContext{.gameObject          = thisObject_,
                                                           .navigationManager   = componentContext_.navigationManager,
                                                           .characterController = *characterController,
                                                           .controller          = *this});

    auto& context = *impl->controllerContext_;
    impl->stateMachine_ =
        std::make_unique<AICharacterFsm>(AIAmbientState{}, AIChaseState{context}, AIAttackState{}, AIQuestState{context});
}
void AIController::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_DETECTION_RADIUS), detectionRadius);
    ::Read(input.getChild(CSTR_TARGETING_MODE), targetingMode);
}

void AIController::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_DETECTION_RADIUS), detectionRadius);
    ::write(node.addChild(CSTR_TARGETING_MODE), targetingMode);
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

    runPerceptionCheck();
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
void AIController::handleEvent(const AIEvent& event)
{
    auto passEventToMachine = [&](const auto& e) { impl->stateMachine_->handle(e); };
    std::visit(passEventToMachine, event);
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
void AIController::runPerceptionCheck()
{
    if (hasTarget and currentTargetHealthComponent)
    {
        float distance = glm::distance(currentTargetHealthComponent->GetParentGameObject().GetWorldTransform().GetPosition(),
                                       thisObject_.GetWorldTransform().GetPosition());

        if (distance > (detectionRadius * 1.2f))
        {
            hasTarget                    = false;
            currentTargetHealthComponent = nullptr;
            pushEventToQueue(TargetLostEvent{});
            return;
        }
    }

    const auto target = getClosestTarget(detectionRadius);

    if (target and not hasTarget)
    {
        hasTarget                    = true;
        currentTargetHealthComponent = target;
        pushEventToQueue(TargetSpottedEvent{target->GetParentGameObject()});
    }
    else if (target and hasTarget and target != currentTargetHealthComponent)
    {
        currentTargetHealthComponent = target;
        pushEventToQueue(TargetSpottedEvent{target->GetParentGameObject()});
    }
    else if (not target and hasTarget)
    {
        hasTarget                    = false;
        currentTargetHealthComponent = nullptr;
        pushEventToQueue(TargetLostEvent{});
    }
}
HealthComponent* AIController::getClosestTarget(float radius)
{
    HealthComponent* result{nullptr};
    auto all = componentContext_.componentController_.GetAllActiveComponentsOfType<HealthComponent>();

    auto minDistance{radius};
    for (auto& hpComponent : all)
    {
        const auto& targetObject = hpComponent->GetParentGameObject();
        if (targetObject.GetId() == thisObject_.GetId())
        {
            continue;
        }

        auto targetStats = targetObject.GetComponent<CharacterStatsComponent>();
        if (not targetStats)
            continue;

        if (not isHostile(*targetStats))
        {
            continue;
        }

        auto distance = glm::distance(hpComponent->GetParentGameObject().GetWorldTransform().GetPosition(),
                                      thisObject_.GetWorldTransform().GetPosition());
        if (distance < minDistance)
        {
            minDistance = distance;
            result      = hpComponent;
        }
    }
    return result;
}
bool AIController::isHostile(const CharacterStatsComponent& targetStats) const
{
    if (not myStats)
        return false;

    const auto attitude = componentContext_.factionManager.getAttitude(myStats->general.faction, targetStats.general.faction);
    return attitude == Attitude::Hostile;
}
float AIController::getAttackRadius() const
{
    return 0.2f;
}
}  // namespace Components
}  // namespace GameEngine
