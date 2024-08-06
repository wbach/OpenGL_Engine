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
    AttackStateBase(FsmContext&, const std::vector<AttackAnimation>&, const std::optional<std::string> = std::nullopt);

    void onEnter(const AttackEvent&);
    void onEnter();

    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void onEnter(const EndMoveLeftEvent&);
    void onEnter(const EndMoveRightEvent&);

    void update(const AttackEvent&);
    void update(const ChangeAnimEvent&);
    void update(float);
    void onLeave(const EndAttackEvent&);
    void onLeave();

    template <typename Event>
    void pushEventToQueue(const Event& event)
    {
        DEBUG_LOG("pushEventToQueue: " + typeName<Event>());
        queue.push_back(event);
    }
    template <typename Event>
    void removeEventFromQueue(const Event& event)
    {
        DEBUG_LOG("removeEventFromQueue: " + typeName<Event>());

        queue.erase(std::remove_if(queue.begin(), queue.end(),
                                   [](const auto& event) { return std::holds_alternative<Event>(event); }));
    }

protected:
    void onClipEnd();
    void subscribe();
    void unsubscribe();

    template <typename... States>
    bool isAnyOfStateQueued();

protected:
    FsmContext& context;
    std::vector<CharacterControllerEvent> queue;

private:
    std::vector<IdType> subIds;
    const std::vector<AttackAnimation>& attackClipNames;

    uint32 sequenceSize     = 0;
    uint32 currentAnimation = 0;

    std::optional<std::string> jointGroupName;
};
}  // namespace Components
}  // namespace GameEngine
