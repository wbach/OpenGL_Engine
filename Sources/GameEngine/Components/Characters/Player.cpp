#include "Player.h"

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType Player::type = ComponentsType::Player;

Player::Player(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , animator_{nullptr}
    , characterController_{nullptr}
    , hurtAnimationName_{"Hurt"}
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
    }
}
void Player::Update()
{
}
void Player::hurt()
{
    DEBUG_LOG("hurt 1");
    if (characterController_)
    {
        characterController_->addState(std::make_unique<Hurt>());
    }
}
void Player::isOnGround()
{
}
}  // namespace Components
}  // namespace GameEngine
