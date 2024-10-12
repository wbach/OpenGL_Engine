#pragma once
#include <Logger/Log.h>

#include <functional>
#include <optional>
#include <string>

#include "../CharacterControllerEvents.h"
#include "BaseState.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
class GameObject;

namespace Components
{
struct FsmContext;
class Rigidbody;

struct JumpStateBase : public BaseState
{
public:
    JumpStateBase(FsmContext&, const std::optional<std::string>&);
    void onEnter(const JumpEvent&);
    void onLeave(const GroundDetectionEvent&);
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

        queue.erase(
            std::remove_if(queue.begin(), queue.end(), [](const auto& event) { return std::holds_alternative<Event>(event); }));
    }

private:
    std::optional<std::string> jointGroupName_;
    std::vector<CharacterControllerEvent> queue;
    float playerCapsuleRadius{0.25f};

    Physics::CollisionSubId collisionSubId;
};
}  // namespace Components
}  // namespace GameEngine
