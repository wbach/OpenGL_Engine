#pragma once
#include "../AnimationClipNames.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class AttackStateBase
{
public:
    AttackStateBase(FsmContext&, const std::vector<AttackAnimation>&);

    void onEnter(const AttackEvent&);
    void update(const AttackEvent&);
    void update(const EndForwardMoveEvent&);
    void update(const EndBackwardMoveEvent&);
    void update(const EndMoveLeftEvent&);
    void update(const EndMoveRightEvent&);
    void update(float);
    void onLeave();

private:
    void onClipEnd();
    void unsubscribe();

private:
    FsmContext& context;

    std::vector<IdType> subIds;
    const std::vector<AttackAnimation>& attackClipNames;

    uint32 sequenceSize     = 0;
    uint32 currentAnimation = 0;

    std::vector<CharacterControllerEvent> queue;
};
}  // namespace Components
}  // namespace GameEngine
