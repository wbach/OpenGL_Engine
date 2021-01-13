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
{
}
void Enemy::CleanUp()
{
}
void Enemy::ReqisterFunctions()
{
}
void Enemy::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();
}
void Enemy::Update()
{
}
void Enemy::isOnGround()
{
}
}  // namespace Components
}  // namespace GameEngine
