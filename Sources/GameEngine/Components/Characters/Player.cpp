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
{
}
void Player::CleanUp()
{
}
void Player::ReqisterFunctions()
{
}
void Player::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();
}
void Player::Update()
{
}
void Player::isOnGround()
{
}
}  // namespace Components
}  // namespace GameEngine
