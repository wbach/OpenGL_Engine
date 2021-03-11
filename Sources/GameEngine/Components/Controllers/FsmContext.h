#pragma once
#include <Types.h>

#include <memory>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext
{
    GameObject& gameObject;
    Physics::IPhysicsApi& physicsApi;
    Rigidbody& rigidbody;
    Animator& animator;

    const std::string& forwardAnimationName;
    const std::string& backwardAnimationName;
    const std::string& jumpAnimationName;
    const std::string& idleAnimationName;

    float moveSpeed{0.f};
    vec3 moveDirection{0.f};

    float rotationSpeed{0.f};
};
}  // namespace Components
}  // namespace GameEngine
