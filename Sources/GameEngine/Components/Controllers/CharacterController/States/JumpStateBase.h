#pragma once
#include <Logger/Log.h>

#include <functional>
#include <optional>
#include <string>

#include "../CharacterControllerEvents.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
class GameObject;

namespace Components
{
struct FsmContext;
struct JumpStateBase
{
public:
    JumpStateBase(FsmContext&, const std::optional<std::string>&);
    void onEnter(const JumpEvent&);
    void update(float);
    void onLeave(const EndJumpEvent&);

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
    void getRigidbodyIdsWhenReady();
    void createGroundChecker();
    void subscribeForGroundCollision();
    void unsubscribeCollisionCallback();
    void subscribeForGroundCollisionWhenIsOnAir();

private:
    FsmContext& context_;
    std::optional<std::string> jointGroupName_;
    std::vector<CharacterControllerEvent> queue;
    GameObject* groundChecker;
    
    Physics::CollisionSubId collisionSubId;
    float collisionSphereRadius{0.3f};
    Physics::RigidbodyId groundCheckerRigidbodyId;
    Physics::RigidbodyId playerRigidbodyId;
};
}  // namespace Components
}  // namespace GameEngine
