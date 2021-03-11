#pragma once
#include <Types.h>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/Rigidbody.h"


namespace GameEngine
{
namespace Components
{
struct FsmContext
{
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
