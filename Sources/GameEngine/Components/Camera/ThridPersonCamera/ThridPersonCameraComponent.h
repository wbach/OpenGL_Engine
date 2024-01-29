#pragma once
#include <Input/KeysSubscriptionsManager.h>
#include "Fsm/Context.h"
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
    ThridPersonCameraComponent(ComponentContext&, GameObject&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    template <typename Event>
    void handleEvent(const Event& event)
    {
        if (fsm)
            fsm->handle(event);
    }

private:
    void init();

private:
    std::unique_ptr<Camera::ThridPersonCameraFsm> fsm;
    std::unique_ptr<Camera::Context> fsmContext;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
