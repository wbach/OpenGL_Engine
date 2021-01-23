#include "Player.h"

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/Characters/Enemy.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/Controllers/ControllerUtlis.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const float ATTACK_RANGE{2.f};
}

ComponentsType Player::type = ComponentsType::Player;

Player::Player(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , hurtAnimationName_{"Hurt"}
    , animator_{nullptr}
    , characterController_{nullptr}
{
}
void Player::CleanUp()
{
}
void Player::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() { Init(); });
}
void Player::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();

    if (animator_)
    {
        auto iter = animator_->animationClips_.find(hurtAnimationName_);
        if (iter != animator_->animationClips_.end())
        {
            iter->second.playType = Animation::AnimationClip::PlayType::once;

            animator_->onAnimationEnd_[hurtAnimationName_].push_back([this]() {
                auto currentAnimationClip = animator_->GetCurrentAnimationName();
                if (currentAnimationClip == hurtAnimationName_)
                {
                    animator_->ChangeAnimation(currentAnimationClip_, Animator::AnimationChangeType::smooth);
                }
            });
        }

        animator_->onAnimationEnd_[characterController_->attackAnimationName].push_back([this]() {
            auto [distance, vectorToPlayer, componentPtr] = getComponentsInRange<Enemy>(
                componentContext_.componentController_, thisObject_.GetWorldTransform().GetPosition());

            if (componentPtr)
            {
                if (distance < ATTACK_RANGE)
                {
                    componentPtr->hurt(characterStatistic_.attackDmg);
                }
            }
        });
    }
}
void Player::Update()
{
}
void Player::hurt(int64 dmg)
{
    DEBUG_LOG("hurt 1");
    if (characterController_)
    {
        characterStatistic_.currentHp -= dmg;
        if (characterStatistic_.currentHp > 0)
        {
            characterController_->addState(std::make_unique<Hurt>());
        }
        else
        {
            characterController_->addState(std::make_unique<Death>());
            characterController_->Deactivate();
        }
    }
}
void Player::isOnGround()
{
}
}  // namespace Components
}  // namespace GameEngine
