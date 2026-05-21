#include "AIController.h"

#include <Logger/Log.h>

#include <memory>

#include "AICharacterFsm.h"
#include "AIControllerContext.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/AI/States/AIAmbientState.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Navigation/NavigationManager.h"
#include "magic_enum/magic_enum.hpp"
namespace GameEngine
{
namespace Components
{
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
    impl->controllerContext_.reset(new AIControllerContext{.gameObject          = thisObject_,
                                                           .navigationManager   = componentContext_.navigationManager,
                                                           .characterController = *characterController,
                                                           .controller          = *this});

    impl->stateMachine_ = std::make_unique<AICharacterFsm>(AIAmbientState{}, AIChaseState{}, AIAttackState{},
                                                           AIQuestState{*impl->controllerContext_});
}
void AIController::read(const TreeNode&)
{
}

void AIController::write(TreeNode& node) const
{
}
void AIController::Update()
{
    if (not IsActive())
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
}  // namespace Components
}  // namespace GameEngine
