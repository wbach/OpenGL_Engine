#include "CharacterController.h"

#include <algorithm>

#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
ComponentsType CharacterController::type = ComponentsType::CharacterController;

CharacterController::CharacterController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , hurtAnimationName{"Hurt"}
    , attackAnimationName{"Attack"}
    , deathAnimationName{"Death"}
    , jumpAnimationName{"Jump"}
    , moveForwardAnimationName{"Run"}
    , moveBackwardAnimationName{}
    , idleAnimationName{"Idle"}
    , rigidbody_{nullptr}
    , jumpPower_(DEFAULT_JUMP_POWER)
    , turnSpeed_(DEFAULT_TURN_SPEED)
    , runSpeed_(DEFAULT_RUN_SPEED)
    , isJumping_(false)
{
}

void CharacterController::CleanUp()
{
}

void CharacterController::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake, std::bind(&CharacterController::Init, this));
    RegisterFunction(FunctionType::Update, std::bind(&CharacterController::Update, this));
}

void CharacterController::Init()
{
    rigidbody_ = thisObject_.GetComponent<Rigidbody>();
    animator_  = thisObject_.GetComponent<Animator>();

    if (rigidbody_)
        rigidbody_->InputParams().angularFactor_ = vec3(0);

    if (animator_)
    {
        animator_->setPlayOnceForAnimationClip(jumpAnimationName);
        animator_->setPlayOnceForAnimationClip(hurtAnimationName);
        animator_->setPlayOnceForAnimationClip(attackAnimationName);
        animator_->setPlayOnceForAnimationClip(deathAnimationName);

        animator_->onAnimationEnd_[hurtAnimationName].push_back([this]() {
            DEBUG_LOG("HURT animation end remove state ");
            removeState(CharacterControllerState::Type::HURT);
        });
        animator_->onAnimationEnd_[attackAnimationName].push_back([this]() {
            DEBUG_LOG("ATTACK animation end remove state ");
            removeState(CharacterControllerState::Type::ATTACK);
        });
    }
}

void CharacterController::Update()
{
    if (not rigidbody_ or not rigidbody_->IsReady())
    {
        return;
    }

    isOnGround();
    processStates();
    clearVelocityIfNotMoving();
}

void CharacterController::addState(std::unique_ptr<CharacterControllerState> state)
{
    std::lock_guard<std::mutex> lk(stateMutex_);

    if (not rigidbody_)
    {
        return;
    }

    state->setup(rigidbody_);
    auto stateIter = findState(state->getType());

    if (stateIter != states_.end())
    {
        if ((*stateIter)->isUpToDate(*state))
        {
            return;
        }
        states_.erase(stateIter);
    }

    if (state->onStart())
    {
        DEBUG_LOG("add state " + std::to_string((int)state->getType()) + " " + thisObject_.GetName());
        states_.push_back(std::move(state));
        onStateChange();
    }
}

void CharacterController::removeState(CharacterControllerState::Type type)
{
    std::lock_guard<std::mutex> lk(stateMutex_);

    auto iter = findState(type);

    if (iter != states_.end())
    {
        DEBUG_LOG("remove state " + std::to_string((int)type) + " " + thisObject_.GetName());
        states_.erase(iter);
        onStateChange();
    }
    else
    {
        // DEBUG_LOG("State not found " + std::to_string((int)type) + " " + thisObject_.GetName());
    }
}

void CharacterController::SetJumpPower(float v)
{
    jumpPower_ = v;
}

void CharacterController::isOnGround()
{
    if (isState(CharacterControllerState::Type::JUMP))
    {
        auto position = thisObject_.GetWorldTransform().GetPosition();
        auto hitTest =
            componentContext_.physicsApi_.RayTest(position + vec3(0, 1.f, 0), vec3(position.x, -10000.f, position.z));

        if (hitTest)
        {
            if (glm::length(position - hitTest->pointWorld) < .1f)
            {
                removeState(CharacterControllerState::Type::JUMP);
            }
        }
    }
}

void CharacterController::onStateChange()
{
    if (isState(CharacterControllerState::Type::DEATH))
    {
        setAnimation(deathAnimationName);
        return;
    }

    if (isState(CharacterControllerState::Type::HURT))
    {
        setAnimation(hurtAnimationName);
        return;
    }

    if (isState(CharacterControllerState::Type::ATTACK))
    {
        setAnimation(attackAnimationName);
        return;
    }

    if (isState(CharacterControllerState::Type::JUMP))
    {
        setAnimation(jumpAnimationName);
        return;
    }

    if (isState(CharacterControllerState::Type::MOVE_FORWARD))
    {
        setAnimation(moveForwardAnimationName);
        return;
    }

    if (isState(CharacterControllerState::Type::MOVE_BACKWARD))
    {
        if (moveBackwardAnimationName.empty())
        {
            setAnimation(moveForwardAnimationName, PlayDirection::backward);
        }
        else
        {
            setAnimation(moveBackwardAnimationName);
        }
        return;
    }

    setAnimation(idleAnimationName);
}

void CharacterController::processStates()
{
    std::lock_guard<std::mutex> lk(stateMutex_);
    for (auto stateIter = states_.begin(); stateIter != states_.end();)
    {
        auto& state = **stateIter;
        state.process(componentContext_.time_.deltaTime);

        if (state.isEnd())
        {
            stateIter = states_.erase(stateIter);
            onStateChange();
        }
        else
        {
            ++stateIter;
        }
    }
}

void CharacterController::clearVelocityIfNotMoving()
{
    if (not isStates({CharacterControllerState::Type::MOVE_FORWARD, CharacterControllerState::Type::MOVE_BACKWARD,
                      CharacterControllerState::Type::JUMP}))
    {
        if (not isJumping_)
        {
            rigidbody_->SetVelocity(vec3(0));
        }
        return;
    }
}

void CharacterController::setAnimation(const std::string& name, PlayDirection direction)
{
    if (animator_ and name != animator_->GetCurrentAnimationName())
    {
        animator_->ChangeAnimation(name, Animator::AnimationChangeType::smooth, direction);
    }
}

bool CharacterController::isState(CharacterControllerState::Type type)
{
    return findState(type) != states_.end();
}

bool CharacterController::isStates(const std::vector<CharacterControllerState::Type>& types)
{
    for (const auto& state : states_)
    {
        auto iter = std::find(types.begin(), types.end(), state->getType());
        if (iter != types.end())
            return true;
    }

    return false;
}

CharacterController::States::iterator CharacterController::findState(CharacterControllerState::Type type)
{
    return std::find_if(states_.begin(), states_.end(), [type](const auto& state) { return state->getType() == type; });
}

void CharacterController::SetTurnSpeed(float v)
{
    turnSpeed_ = v;
}

void CharacterController::clearStates()
{
    std::lock_guard<std::mutex> lk(stateMutex_);
    states_.clear();
}

void CharacterController::SetRunSpeed(float v)
{
    runSpeed_ = v;
}
}  // namespace Components
}  // namespace GameEngine
