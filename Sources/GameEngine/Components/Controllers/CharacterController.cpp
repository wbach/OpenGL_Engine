#include "CharacterController.h"

#include <algorithm>

#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::unordered_map<CharacterControllerState::Type, uint8> statePriorities{
    // clang-format off
    {CharacterControllerState::Type::DEATH, 0},
    {CharacterControllerState::Type::HURT, 1},
    {CharacterControllerState::Type::ATTACK, 2},
    {CharacterControllerState::Type::JUMP, 3},
    {CharacterControllerState::Type::MOVE_FORWARD, 4},
    {CharacterControllerState::Type::MOVE_BACKWARD, 5},
    {CharacterControllerState::Type::MOVE_LEFT, 6},
    {CharacterControllerState::Type::MOVE_RIGHT, 7},
    {CharacterControllerState::Type::ROTATE_LEFT, 8},
    {CharacterControllerState::Type::ROTATE_RIGHT, 9},
    {CharacterControllerState::Type::ROTATE_TARGET, 10},
    {CharacterControllerState::Type::IDLE, 11}
    // clang-format on
};
}
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
{
    stateTypeToAnimName_ = {{CharacterControllerState::Type::DEATH, &deathAnimationName},
                            {CharacterControllerState::Type::HURT, &hurtAnimationName},
                            {CharacterControllerState::Type::ATTACK, &attackAnimationName},
                            {CharacterControllerState::Type::JUMP, &jumpAnimationName},
                            {CharacterControllerState::Type::MOVE_FORWARD, &moveForwardAnimationName},
                            //{CharacterControllerState::Type::MOVE_BACKWARD, &moveBackwardAnimationName},
                            //{CharacterControllerState::Type::MOVE_LEFT, 6},
                            //{CharacterControllerState::Type::MOVE_RIGHT, 7},
                            //{CharacterControllerState::Type::ROTATE_LEFT, 8},
                            //{CharacterControllerState::Type::ROTATE_RIGHT, 9},
                            //{CharacterControllerState::Type::ROTATE_TARGET, 10},
                            {CharacterControllerState::Type::IDLE, &idleAnimationName}};
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

        animator_->onAnimationEnd_[hurtAnimationName].push_back(
            [this]() { removeState(CharacterControllerState::Type::HURT); });
        animator_->onAnimationEnd_[attackAnimationName].push_back(
            [this]() { removeState(CharacterControllerState::Type::ATTACK); });
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
        auto type = state->getType();
        states_.push_back(std::move(state));
        onStateAdittion(type);
    }
}

void CharacterController::removeState(CharacterControllerState::Type type)
{
    std::lock_guard<std::mutex> lk(stateMutex_);

    auto iter = findState(type);

    if (iter != states_.end())
    {
        states_.erase(iter);
        onStateRemove(type);
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

void CharacterController::onStateAdittion(CharacterControllerState::Type type)
{
    auto incomingPrioIter = statePriorities.find(type);
    auto changeAnimation =
        std::all_of(states_.begin(), states_.end(), [incomingPrio = incomingPrioIter->second](const auto& state) {
            auto prioIter = statePriorities.find(state->getType());
            return incomingPrio <= prioIter->second;
        });

    if (changeAnimation)
    {
        auto animNameIter = stateTypeToAnimName_.find(type);

        if (animNameIter != stateTypeToAnimName_.end() and not animNameIter->second->empty())
        {
            setAnimation(*animNameIter->second);
        }
        else if (type == CharacterControllerState::Type::MOVE_BACKWARD and not moveForwardAnimationName.empty())
        {
            setAnimation(moveForwardAnimationName, PlayDirection::backward);
        }
    }
}

void CharacterController::onStateRemove(CharacterControllerState::Type type)
{
    auto incomingPrioIter = statePriorities.find(type);

    auto doNotchangeAnimation =
        std::any_of(states_.begin(), states_.end(), [incomingPrio = incomingPrioIter->second](const auto& state) {
            auto prioIter = statePriorities.find(state->getType());
            return (incomingPrio > prioIter->second);
        });

    if (doNotchangeAnimation)
    {
        return;
    }

    if (not states_.empty())
    {
        auto result = std::min_element(states_.begin(), states_.end(), [](const auto& l, const auto& r) {
            auto lp = statePriorities.find(l->getType());
            auto rp = statePriorities.find(r->getType());

            if (lp != statePriorities.end() and rp != statePriorities.end())
            {
                return lp->second < rp->second;
            }
            return false;
        });

        auto animNameIter = stateTypeToAnimName_.find((**result).getType());
        if (animNameIter != stateTypeToAnimName_.end() and not animNameIter->second->empty())
        {
            setAnimation(*animNameIter->second);
            return;
        }
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
            auto type = state.getType();
            stateIter = states_.erase(stateIter);
            onStateRemove(type);
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
        rigidbody_->SetVelocity(vec3(0));
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
