#include "Enemy.h"

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
ComponentsType Enemy::type = ComponentsType::Enemy;

Enemy::Enemy(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(type, componentContext, gameObject)
    , characterController_{nullptr}
    , animator_{nullptr}
{
}
void Enemy::CleanUp()
{
}
void Enemy::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() { Init(); });
}
void Enemy::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();

    if (animator_)
    {
        auto iter = animator_->animationClips_.find(characterController_->hurtAnimationName);
        if (iter != animator_->animationClips_.end())
        {
            iter->second.playType = Animation::AnimationClip::PlayType::once;

            animator_->onAnimationEnd_[characterController_->hurtAnimationName].push_back([this]() {
                auto currentAnimationClip = animator_->GetCurrentAnimationName();
                if (currentAnimationClip == characterController_->hurtAnimationName)
                {
                    animator_->ChangeAnimation(characterController_->hurtAnimationName,
                                               Animator::AnimationChangeType::smooth);
                }
            });
        }
    }
}
void Enemy::Update()
{
}
void Enemy::hurt()
{
    if (characterController_)
    {
        DEBUG_LOG("");
        characterController_->addState(std::make_unique<Hurt>());
    }
}
void Enemy::isOnGround()
{
}
}  // namespace Components
}  // namespace GameEngine
