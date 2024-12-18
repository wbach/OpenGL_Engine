#pragma once
#include <optional>
#include <string>

#include "../AnimationClipNames.h"
#include "../CharacterControllerEvents.h"
#include "BaseState.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;
class ThridPersonCameraComponent;

class RotateStateBase
{
public:
    enum class CameraRotationPolicy
    {
        none,
        rotateOnlyCameraIfAvaiable
    };
    RotateStateBase(FsmContext&, const std::optional<std::string>&, float, const RotateAnimationClips&,
                    CameraRotationPolicy = CameraRotationPolicy::rotateOnlyCameraIfAvaiable);

    void onEnter(const CrouchChangeStateEvent&);
    void onEnter(const EndAttackEvent&);
    void onEnter(const EquipEndStateEvent&);
    void onEnter(const DisarmEndStateEvent&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void onEnter(const EndMoveLeftEvent&);
    void onEnter(const EndMoveRightEvent&);
    void onEnter(const AimStopEvent&);
    void update(float);

    void update(const RotateLeftEvent&);
    void update(const RotateRightEvent&);
    void update(const RotateTargetEvent&);
    void update(const AimStartEvent&);

    void setRotateLeftAnim();
    void setRotateRightAnim();
    void setCurrentAnim();
    void setCurrentRotation();
    void setCurrentAnimAndRotation();

    void updateAnimationClipNames(const RotateAnimationClips&);
    const RotateAnimationClips& getCurrentAnimationClips() const;

protected:
    FsmContext& context_;
    std::optional<std::string> jointGroupName_;
    RotateAnimationClips rotateAnimationClips_;
    float rotateSpeed_;
    ThridPersonCameraComponent* cameraComponent_;
    CameraRotationPolicy cameraRotationPolicy_;
};
}  // namespace Components
}  // namespace GameEngine
