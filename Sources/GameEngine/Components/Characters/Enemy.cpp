#include "Enemy.h"

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/Characters/Player.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/ControllerUtlis.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR{"Enemy"};
}  // namespace

Enemy::Enemy(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(Enemy).hash_code(), componentContext, gameObject)
    , animator_{nullptr}
    , characterController_{nullptr}
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

    if (animator_ and characterController_)
    {
        auto iter = animator_->animationClips_.find(characterController_->hurtAnimationName);
        if (iter != animator_->animationClips_.end())
        {
            iter->second.playType = Animation::AnimationClip::PlayType::once;
        }

        animator_->onAnimationEnd_[characterController_->attackAnimationName].push_back([this]() {
            auto [distance, vectorToPlayer, componentPtr] = getComponentsInRange<Player>(
                componentContext_.componentController_, thisObject_.GetWorldTransform().GetPosition());

            if (componentPtr)
            {
                if (distance < characterStatistic_.attackRange)
                {
                    componentPtr->hurt(characterStatistic_.attackDmg);
                }
            }
        });
    }
}
void Enemy::Update()
{
}
void Enemy::hurt(int64 dmg)
{
    if (characterController_)
    {
        DEBUG_LOG("");
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

const CharacterStatistic& Enemy::characterStatistic() const
{
    return characterStatistic_;
}
void Enemy::isOnGround()
{
}
void Enemy::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode&, GameObject& gameObject) {
        return std::make_unique<Enemy>(componentContext, gameObject);
    };
    ReadFunctions::instance().componentsReadFunctions.insert({COMPONENT_STR, readFunc});
}
void Enemy::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}
}  // namespace Components
}  // namespace GameEngine
