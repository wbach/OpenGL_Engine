#include "CharacterController.h"

#include <algorithm>

#include "GameEngine/Components/CommonReadDef.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR               = "CharacterController";
const std::string CSTR_IDLE_ANIMATION         = "idle";
const std::string CSTR_RUN_ANIMATION          = "run";
const std::string CSTR_MOVEBACKWARD_ANIMATION = "moveBackward";
const std::string CSTR_WALK_ANIMATION         = "walk";
const std::string CSTR_ATTACK_ANIMATION       = "attack";
const std::string CSTR_ATTACK_ANIMATION2      = "attack2";
const std::string CSTR_ATTACK_ANIMATION3      = "attack3";
const std::string CSTR_HURT_ANIMATION         = "hurt";
const std::string CSTR_DEATH_ANIMATION        = "death";
const std::string CSTR_JUMP_ANIMATION         = "jump";

const std::unordered_map<CharacterControllerState::Type, uint8> statePriorities{
    // clang-format off
    {CharacterControllerState::Type::DEATH, 0},
    {CharacterControllerState::Type::ATTACK, 1},
    {CharacterControllerState::Type::HURT, 2},
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
}  // namespace
CharacterController::CharacterController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(CharacterController).hash_code(), componentContext, gameObject)
    , hurtAnimationName{"Hurt"}
    , attackAnimationName{"Attack"}
    , deathAnimationName{"Death"}
    , jumpAnimationName{"Jump"}
    , moveForwardAnimationName{"Run"}
    , moveBackwardAnimationName{}
    , idleAnimationName{"Idle"}
    , upperBodyGroupName{"upperBody"}
    , lowerBodyGroupName{"lowerBody"}
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
        animator_->setPlayOnceForAnimationClip(deathAnimationName);
        animator_->setPlayOnceForAnimationClip(attackAnimationName);
        animator_->setPlayOnceForAnimationClip(attackAnimationName2);
        animator_->setPlayOnceForAnimationClip(attackAnimationName3);

        if (not hurtAnimationName.empty())
        {
            animator_->onAnimationEnd_[hurtAnimationName].push_back(
                [this]() { removeState(CharacterControllerState::Type::HURT); });
        }

        if (not attackAnimationName.empty())
        {
            animator_->onAnimationEnd_[attackAnimationName].push_back([this]() {
                DEBUG_LOG("attack 1 end");
                if (not attackAnimationName2.empty())
                {
                    auto iter = stateTypeToAnimName_.find(CharacterControllerState::Type::ATTACK);
                    if (iter != stateTypeToAnimName_.end())
                    {
                        iter->second = &attackAnimationName2;

                        attackTimer_.SetCallback(
                            1000, [iter, attack1namePtr = &attackAnimationName]() { iter->second = attack1namePtr; });
                    }
                }

                removeState(CharacterControllerState::Type::ATTACK);
            });
        }

        if (not attackAnimationName2.empty())
        {
            DEBUG_LOG("attack 2 end");
            animator_->onAnimationEnd_[attackAnimationName2].push_back([this]() {
                if (not attackAnimationName3.empty())
                {
                    auto iter = stateTypeToAnimName_.find(CharacterControllerState::Type::ATTACK);
                    if (iter != stateTypeToAnimName_.end())
                    {
                        iter->second = &attackAnimationName3;

                        attackTimer_.SetCallback(
                            1000, [iter, attack1namePtr = &attackAnimationName]() { iter->second = attack1namePtr; });
                    }
                }
                else
                {
                    auto iter = stateTypeToAnimName_.find(CharacterControllerState::Type::ATTACK);
                    if (iter != stateTypeToAnimName_.end())
                    {
                        iter->second = &attackAnimationName;
                    }
                    attackTimer_.Reset();
                }

                removeState(CharacterControllerState::Type::ATTACK);
            });
        }

        if (not attackAnimationName3.empty())
        {
            DEBUG_LOG("attack 3 end");
            animator_->onAnimationEnd_[attackAnimationName3].push_back([this]() {
                auto iter = stateTypeToAnimName_.find(CharacterControllerState::Type::ATTACK);
                if (iter != stateTypeToAnimName_.end())
                {
                    iter->second = &attackAnimationName;
                }
                attackTimer_.Reset();
                removeState(CharacterControllerState::Type::ATTACK);
            });
        }
    }
}

void CharacterController::Update()
{
    if (not rigidbody_ or not rigidbody_->IsReady())
    {
        return;
    }

    attackTimer_.Update();
    isOnGround();
    processStates();
    clearVelocityIfNotMoving();
}

bool CharacterController::addState(std::unique_ptr<CharacterControllerState> state)
{
    std::lock_guard<std::mutex> lk(stateMutex_);

    if (not rigidbody_)
    {
        return false;
    }

    state->setup(rigidbody_);
    auto stateIter = findState(state->getType());

    if (stateIter != states_.end())
    {
        if ((*stateIter)->isUpToDate(*state))
        {
            return false;
        }
        states_.erase(stateIter);
    }

    if (state->onStart())
    {
        auto type = state->getType();
        states_.push_back(std::move(state));
        onStateAdittion(type);
    }

    return true;
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
        const auto& position = thisObject_.GetWorldTransform().GetPosition();
        auto hitTest         = componentContext_.physicsApi_.RayTest(position, vec3(position.x, -10000.f, position.z));

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
    if (type == CharacterControllerState::Type::ROTATE_LEFT or type == CharacterControllerState::Type::ROTATE_RIGHT or
        type == CharacterControllerState::Type::ROTATE_TARGET)
        return;

    if (type == CharacterControllerState::Type::DEATH)
    {
        setAnimation(deathAnimationName);
        return;
    }

    auto animNameIter = stateTypeToAnimName_.find(type);

    if (type == CharacterControllerState::Type::ATTACK)
    {
        auto isMoving = std::any_of(states_.begin(), states_.end(), [](const auto& state) {
            return state->getType() == CharacterControllerState::Type::MOVE_FORWARD or
                   state->getType() == CharacterControllerState::Type::MOVE_BACKWARD or
                   state->getType() == CharacterControllerState::Type::JUMP;
        });

        if (not isMoving)
        {
            setAnimation(*animNameIter->second);
        }
        else
        {
            setAnimation(*animNameIter->second, PlayDirection::forward, upperBodyGroupName);
        }

        return;
    }

    if (type == CharacterControllerState::Type::MOVE_FORWARD or type == CharacterControllerState::Type::MOVE_BACKWARD or
        type == CharacterControllerState::Type::JUMP)
    {
        auto isAttacking = std::any_of(states_.begin(), states_.end(), [](const auto& state) {
            return state->getType() == CharacterControllerState::Type::ATTACK;
        });

        if (not isAttacking)
        {
            setAnimation(*animNameIter->second);
        }
        else
        {
            setAnimation(*animNameIter->second, PlayDirection::forward, lowerBodyGroupName);
        }

        return;
    }

    setAnimation(idleAnimationName);

    //    auto incomingPrioIter = statePriorities.find(type);

    //    if (getParentGameObject().GetName() == "Aang")
    //    {
    //        DEBUG_LOG(std::to_string((int)type) + " incomingPrioIter" + std::to_string(incomingPrioIter->second));

    //        for(auto& state : states_)
    //        {
    //            auto prioIter = statePriorities.find(state->getType());
    //            DEBUG_LOG(std::to_string((int)state->getType()) + " incomingPrioIter" +
    //            std::to_string(prioIter->second));
    //        }
    //    }

    //    auto changeAnimation =
    //        std::all_of(states_.begin(), states_.end(), [incomingPrio = incomingPrioIter->second](const auto& state) {
    //            auto prioIter = statePriorities.find(state->getType());
    //            return incomingPrio <= prioIter->second;
    //        });

    //    if (changeAnimation)
    //    {
    //        auto animNameIter = stateTypeToAnimName_.find(type);

    //        if (animNameIter != stateTypeToAnimName_.end())
    //            DEBUG_LOG(*animNameIter->second);
    //        else
    //            DEBUG_LOG(std::to_string((int)type));

    //        if (animNameIter != stateTypeToAnimName_.end() and not animNameIter->second->empty())
    //        {
    //            setAnimation(*animNameIter->second);
    //        }
    //        else if (type == CharacterControllerState::Type::MOVE_BACKWARD and not moveForwardAnimationName.empty())
    //        {
    //            setAnimation(moveForwardAnimationName, PlayDirection::backward);
    //        }
    //    }
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
        rigidbody_->SetVelocity(vec3(0.f, rigidbody_->GetVelocity().y, 0.f));
    }
}

void CharacterController::setAnimation(const std::string& name, PlayDirection direction,
                                       std::optional<std::string> groupName)
{
    if (animator_ and name != currentAnimation)
    {
        animator_->ChangeAnimation(name, Animator::AnimationChangeType::smooth, direction, groupName);
        currentAnimation = name;
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
void CharacterController::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode& node, GameObject& gameObject) {
        auto component = std::make_unique<CharacterController>(componentContext, gameObject);

        auto animationClipsNode = node.getChild(CSTR_ANIMATION_CLIPS);
        if (animationClipsNode)
        {
            ::Read(animationClipsNode->getChild(CSTR_IDLE_ANIMATION), component->idleAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_HURT_ANIMATION), component->hurtAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_RUN_ANIMATION), component->moveForwardAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_MOVEBACKWARD_ANIMATION), component->moveBackwardAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_DEATH_ANIMATION), component->deathAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_ATTACK_ANIMATION), component->attackAnimationName);
            ::Read(animationClipsNode->getChild(CSTR_ATTACK_ANIMATION2), component->attackAnimationName2);
            ::Read(animationClipsNode->getChild(CSTR_ATTACK_ANIMATION3), component->attackAnimationName3);
            ::Read(animationClipsNode->getChild(CSTR_JUMP_ANIMATION), component->jumpAnimationName);
        }
        return component;
    };

    regsiterComponentReadFunction(COMPONENT_STR, readFunc);
}

void CharacterController::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
    auto& animClipsNode = node.addChild(CSTR_ANIMATION_CLIPS);
    ::write(animClipsNode.addChild(CSTR_IDLE_ANIMATION), idleAnimationName);
    ::write(animClipsNode.addChild(CSTR_HURT_ANIMATION), hurtAnimationName);
    ::write(animClipsNode.addChild(CSTR_RUN_ANIMATION), moveForwardAnimationName);
    ::write(animClipsNode.addChild(CSTR_MOVEBACKWARD_ANIMATION), moveBackwardAnimationName);
    ::write(animClipsNode.addChild(CSTR_DEATH_ANIMATION), deathAnimationName);
    ::write(animClipsNode.addChild(CSTR_ATTACK_ANIMATION), attackAnimationName);
    ::write(animClipsNode.addChild(CSTR_ATTACK_ANIMATION2), attackAnimationName2);
    ::write(animClipsNode.addChild(CSTR_ATTACK_ANIMATION3), attackAnimationName3);
    ::write(animClipsNode.addChild(CSTR_JUMP_ANIMATION), jumpAnimationName);
}
}  // namespace Components
}  // namespace GameEngine
