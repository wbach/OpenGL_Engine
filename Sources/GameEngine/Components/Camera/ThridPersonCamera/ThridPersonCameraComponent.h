#pragma once
#include <Input/KeysSubscriptionsManager.h>

#include <optional>

#include "Fsm/Context.h"
#include "Fsm/ThridPersonCameraEvents.h"
#include "Fsm/ThridPersonCameraFsm.h"
#include "GameEngine/Components/BaseComponent.h"
#include "Logger/Log.h"
#include "Types.h"

namespace GameEngine
{
class ICamera;
class ThirdPersonCamera;

namespace Components
{
class ThridPersonCameraComponent : public BaseComponent
{
public:
    vec3 runLocalCameraPos{-0.5f, 1.0f, -1.5f};
    vec3 aimLocalCameraPos{-0.25f, 1.f, -0.75f};

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_VECTOR3F(runLocalCameraPos)
        FIELD_VECTOR3F(aimLocalCameraPos)
    END_FIELDS()
    // clang-format on

public:
    ThridPersonCameraComponent(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

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

    void pushEventToQueue(const Camera::Event& event)
    {
        eventQueue.push_back(event);
    }

    std::tuple<float, float> getRotation() const;
    vec3 getDirection() const;

private:
    void awake();
    void init();
    void processEvent();

public:
    std::unique_ptr<Camera::ThridPersonCameraFsm> fsm;
    std::unique_ptr<Camera::Context> fsmContext;

private:
    std::vector<Camera::Event> eventQueue;
    std::optional<IdType> subscribtionOnTransformSnapshot;
    std::optional<IdType> cameraId;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
