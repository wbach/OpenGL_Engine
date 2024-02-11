#pragma once
#include <string>
#include <optional>

#include "../CharacterControllerEvents.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;

class RotateStateBase
{
public:
    RotateStateBase(FsmContext&, const std::optional<std::string>&, float, const std::string&, const std::string&);

    void onEnter(const EquipEndStateEvent&);
    void onEnter(const DisarmEndStateEvent&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void onEnter(const AimStopEvent&);
    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
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

    void updateAnimationClipNames(const std::string&, const std::string&);

protected:
    FsmContext& context_;
    std::optional<std::string> jointGroupName_;
    std::string rotateLeftAnim_;
    std::string rotateRightAnim_;
    float rotateSpeed_;
};
}  // namespace Components
}  // namespace GameEngine
