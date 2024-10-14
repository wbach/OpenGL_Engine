#pragma once
#include "../AnimationClipNames.h"
#include "BaseState.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
struct AttackStatesContext;
class DisarmedRunState;
class DisarmedWalkState;
class ArmedRunState;
class ArmedWalkState;

class AttackStateBase : public BaseState
{
public:
    AttackStateBase(FsmContext&, const std::vector<AttackAnimation>&, const std::optional<std::string> = std::nullopt);

    void onEnter(DisarmedRunState&);
    void onEnter(DisarmedWalkState&);
    void onEnter(ArmedRunState&);
    void onEnter(ArmedWalkState&);
    void onEnter(const AttackEvent&);
    void onEnter();

    void onEnter(const EndRotationEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void onEnter(const EndMoveLeftEvent&);
    void onEnter(const EndMoveRightEvent&);
    void onEnter(const ChangeAnimEvent&);

    void update(const AttackEvent&);
    void update(const ChangeAnimEvent&);
    void update(float);
    void onLeave(const EndAttackEvent&);
    void onLeave();

protected:
    void onClipEnd();
    void subscribe();
    void unsubscribe();
    const AttackAnimation& getCurrentAttackAnimation() const;

private:
    std::vector<IdType> subIds;
    const std::vector<AttackAnimation>& attackClipNames;

    std::optional<std::string> jointGroupName;
};
}  // namespace Components
}  // namespace GameEngine
