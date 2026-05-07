#include "Player.h"

#include <Utils/TreeNodeWriteFunctions.h>
#include <Utils/TreeNodeReadFunctions.h>

#include "Common/Controllers/CharacterController/Character.h"
#include "Common/Controllers/CharacterController/CharacterActions.h"
#include "Common/Transform.h"
#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Camera/CameraManager.h"
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Characters/Enemy.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/ControllerUtlis.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/Animation.h"
#include "GameEngine/Renderers/GUI/Element.h"
#include "GameEngine/Renderers/GUI/IElementFactory.h"
#include "GameEngine/Renderers/GUI/Layer/DefaultLayers.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Manager.h"
#include "GameEngine/Renderers/GUI/Transform.h"
#include "GameEngine/Renderers/GUI/Window/Window.h"
#include "GameEngine/Renderers/RenderersManager.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR{"Player"};
const float ATTACK_RANGE{2.f};

vec2 convertToNdcPosition(const mat4& viewProjectionMatrix, const vec3& point)
{
    auto clipSpace = viewProjectionMatrix * vec4(point, 1.f);
    return vec2(clipSpace.x, clipSpace.y) / clipSpace.w;
}

vec2 convertToScreenPosition(const mat4& viewProjectionMatrix, const vec3& point)
{
    return convertToNdcPosition(viewProjectionMatrix, point) / 2.f + 0.5f;
}
}  // namespace

Player::Player(ComponentContext& componentContext, GameObject& gameObject)
    : ComponentCore(GetComponentType<Player>(), componentContext, gameObject)
    , guiManager_{componentContext.guiManager_}
    , animator_{nullptr}
    , characterController_{nullptr}
    , hudElements_{{characterStatistic_.currentHp, characterStatistic_.maxHp},
                   {characterStatistic_.currentStamina, characterStatistic_.maxStamina},
                   {characterStatistic_.currentMana, characterStatistic_.maxMana}}
{
}
void Player::CleanUp()
{
    if (hudElements_.window)
    {
        guiManager_.remove(*hudElements_.window);
    }

    for (auto& id : animSubs_)
        animator_->UnSubscribeForAnimationFrame(id);
}
void Player::ReqisterFunctions()
{
    RegisterFunction(
        FunctionType::OnStart, [this]() { Init(); }, MakeDependencies<CharacterController>());
    RegisterFunction(FunctionType::Update, [this]() { Update(); });
}
void Player::Reload()
{
}
void Player::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();

    if (animator_ and characterController_)
    {
        auto attackAction = [this]()
        {
            auto [distance, vectorToPlayer, componentPtr] = getComponentsInRange<Enemy>(
                componentContext_.componentController_, thisObject_.GetWorldTransform().GetPosition());

            if (componentPtr)
            {
                if (distance < ATTACK_RANGE + characterController_->getShapeSize())
                {
                    auto dmg = componentPtr->hurt(characterStatistic_.attackDmg);

                    if (dmg)
                    {
                        renderDmg(componentPtr->getParentGameObject().GetWorldTransform(), *dmg);
                    }
                }
            }
        };
        // for (const auto& attackAnim : characterController_->animationClipsNames_.armed.attack)
        // {
        //     if (auto maybeId = animator_->SubscribeForAnimationFrame(attackAnim.name, attackAction))
        //     {
        //         animSubs_.push_back(*maybeId);
        //     }
        // }
        // for (const auto& attackAnim : characterController_->animationClipsNames_.disarmed.attack)
        // {
        //     if (auto maybeId = animator_->SubscribeForAnimationFrame(attackAnim.name, attackAction))
        //     {
        //         animSubs_.push_back(*maybeId);
        //     }
        // }
    }

    const vec2 windowSize(0.2f, 0.1f);
    const vec2 windowPosition(0.01f);

    auto window = componentContext_.guiElementFactory_.createWindow(GUI::WindowStyle::BACKGROUND_ONLY);

    window->setTransform(GUI::Transform{.position = windowPosition + 0.5f * windowSize, .scale = windowSize});

    hudElements_.window = window.get();
    auto verticalLayout = componentContext_.guiElementFactory_.createVerticalLayout();
    verticalLayout->setAlign(GUI::HorizontalAlign::LEFT);

    auto hpRedBar           = componentContext_.guiElementFactory_.createSprite("GUI/Health_Bars/HP/Style_1.png");
    hudElements_.hp.texture = hpRedBar.get();
    hpRedBar->setLocalScale({1.f, 0.33f});
    verticalLayout->addChild(std::move(hpRedBar));

    auto staminaRedBar           = componentContext_.guiElementFactory_.createSprite("GUI/yellow.jpg");
    hudElements_.stamina.texture = staminaRedBar.get();
    staminaRedBar->setLocalScale({1.f, 0.33f});
    verticalLayout->addChild(std::move(staminaRedBar));

    auto manaBar              = componentContext_.guiElementFactory_.createSprite("GUI/blue.jpg");
    hudElements_.mana.texture = manaBar.get();
    manaBar->setLocalScale({1.f, 0.33f});
    verticalLayout->addChild(std::move(manaBar));

    auto frame = componentContext_.guiElementFactory_.createSprite("GUI/liberated_frame_video.png");
    window->addChild(std::move(frame));
    window->addChild(std::move(verticalLayout));
    auto& layer = guiManager_.createLayer(DefaultGuiLayers::hud);
    layer.add(std::move(window));
}
void Player::Update()
{
    hudElements_.hp.update();
    hudElements_.stamina.update();
    hudElements_.mana.update();
}
void Player::hurt(int64 dmg)
{
    if (characterController_ and characterStatistic_.currentHp > 0)
    {
        characterStatistic_.currentHp -= dmg;
        if (characterStatistic_.currentHp > 0)
        {
            // characterController_->addState(std::make_unique<Hurt>());
        }
        else
        {
            characterController_->pushEventToQueue(DeathEvent{});
            characterController_->Deactivate();
            Deactivate();
        }
    }
}
void Player::renderDmg(const common::Transform& enemyTransform, int64 dmg)
{
    vec2 offset(getRandomFloat() / 20.f, getRandomFloat() / 20.f);
    auto hitInfoWorldPoision = enemyTransform.GetPosition() + vec3(0, enemyTransform.GetScale().y / 2.f, 0);
    auto camera              = componentContext_.renderersManager_.GetContext().camera_;
    if (not camera)
    {
        return;
    }
    const auto& projectionViewMatrix = camera->GetProjectionViewMatrix();
    auto hitInfoScreenPosition       = convertToScreenPosition(projectionViewMatrix, hitInfoWorldPoision);
    auto hitText                     = componentContext_.guiElementFactory_.createText(std::to_string(dmg));
    hitText->setScreenScale(vec2(0.05f));
    hitText->setScreenPostion(hitInfoScreenPosition + offset);
    hitText->setColor(vec4(1.f, 1.f, 1.f, 0.f));

    auto hitTextPtr = hitText.get();

    guiManager_.add(
        GUI::Animation(std::move(hitText), GUI::Animation::Duration(1.f),
                     [offset, hitTextPtr, &enemyTransform, &componentContext = componentContext_](
                         GUI::Element& text, GUI::Animation::DeltaTime deltaTime, GUI::Animation::Duration elapsedTime) mutable
                     {
                         if (elapsedTime > 0.5f)
                         {
                             const float speed    = 0.05f;
                             auto currentPosition = text.getScreenPosition();
                             currentPosition.y += (speed * deltaTime);
                             offset.y += (speed * deltaTime);
                         }

                         auto hitInfoWorldPoision = enemyTransform.GetPosition() + vec3(0, enemyTransform.GetScale().y / 2.f, 0);

                         auto camera = componentContext.renderersManager_.GetContext().camera_;
                         if (not camera)
                         {
                             return;
                         }
                         const auto& projectionViewMatrix = camera->GetProjectionViewMatrix();
                         auto hitInfoScreenPosition       = convertToScreenPosition(projectionViewMatrix, hitInfoWorldPoision);
                         text.setScreenPostion(hitInfoScreenPosition + offset);

                         auto currentColor = hitTextPtr->getColor();
                         if (elapsedTime < 0.4f)
                         {
                             if (currentColor.a() < 1.f)
                             {
                                 currentColor.a(currentColor.a() + 3.f * deltaTime);
                             }
                         }
                         else if (elapsedTime > 0.6f)
                         {
                             if (currentColor.a() > 0.f)
                             {
                                 currentColor.a(currentColor.a() - 3.f * deltaTime);
                             }
                         }
                         hitTextPtr->setColor(currentColor);
                     }));
}
void Player::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode&, GameObject& gameObject)
    { return std::make_unique<Player>(componentContext, gameObject); };
    regsiterComponentReadFunction(GetComponentType<Player>(), readFunc);
}
void Player::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}
}  // namespace Components
}  // namespace GameEngine
