#pragma once
#include <Input/KeysSubscriptionsManager.h>
#include "Fsm/Context.h"
#include "Fsm/ThridPersonCameraEvents.h"
#include "Fsm/ThridPersonCameraFsm.h"
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
struct ICamera;
class ThirdPersonCamera;

namespace Components
{
class ThridPersonCameraComponent : public BaseComponent
{
public:
    using Event = std::variant<Camera::StartAimEvent, Camera::StopAimEvent>;

    ThridPersonCameraComponent(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    template <typename Event>
    void handleEvent(const Event& event)
    {
        if (fsm)
            fsm->handle(event);
    }

    template <typename Event>
    void pushEventToQueue(const Event& event)
    {
        eventQueue.push_back(event);
    }

    void pushEventToQueue(const Event& event)
    {
        eventQueue.push_back(event);
    }

private:
    void init();
    void processEvent();

public:
    std::unique_ptr<Camera::ThridPersonCameraFsm> fsm;
    std::unique_ptr<Camera::Context> fsmContext;

private:
    std::vector<Event> eventQueue;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
