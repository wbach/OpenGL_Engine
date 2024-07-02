#include "Player.h"

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/Characters/Enemy.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/ControllerUtlis.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/GuiAnimation.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/GUI/Layer/DefaultLayers.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Renderers/RenderersManager.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string COMPONENT_STR{"Player"};
const float ATTACK_RANGE{2.f};
}  // namespace
Player::Player(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(Player).hash_code(), componentContext, gameObject)
    , guiManager_{componentContext.guiElementFactory_.getManager()}
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
        guiManager_.Remove(*hudElements_.window);
    }

    for (auto& id : animSubs_)
        animator_->UnSubscribeForAnimationFrame(id);
}
void Player::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() { Init(); });
    RegisterFunction(FunctionType::Update, [this]() { Update(); });
}
void Player::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();

    if (animator_ and characterController_)
    {
        auto attackAction = [this]() {
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
        for (const auto& attackAnim : characterController_->animationClipsNames_.armed.attack)
        {
            animSubs_.push_back(animator_->SubscribeForAnimationFrame(attackAnim.name, attackAction));
        }
        for (const auto& attackAnim : characterController_->animationClipsNames_.disarmed.attack)
        {
            animSubs_.push_back(animator_->SubscribeForAnimationFrame(attackAnim.name, attackAction));
        }
    }

    const vec2 windowSize(0.2f, 0.1f);
    const vec2 windowPosition(0.01f);

    auto window = componentContext_.guiElementFactory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY,
                                                                       windowPosition + 0.5f * windowSize, windowSize);

    hudElements_.window = window.get();
    auto verticalLayout = componentContext_.guiElementFactory_.CreateVerticalLayout();
    verticalLayout->SetAlgin(Layout::Algin::LEFT);

    auto hpRedBar           = componentContext_.guiElementFactory_.CreateGuiTexture("GUI/Health_Bars/HP/Style_1.png");
    hudElements_.hp.texture = hpRedBar.get();
    hpRedBar->SetLocalScale({1.f, 0.33f});
    verticalLayout->AddChild(std::move(hpRedBar));

    auto staminaRedBar           = componentContext_.guiElementFactory_.CreateGuiTexture("GUI/yellow.jpg");
    hudElements_.stamina.texture = staminaRedBar.get();
    staminaRedBar->SetLocalScale({1.f, 0.33f});
    verticalLayout->AddChild(std::move(staminaRedBar));

    auto manaBar              = componentContext_.guiElementFactory_.CreateGuiTexture("GUI/blue.jpg");
    hudElements_.mana.texture = manaBar.get();
    manaBar->SetLocalScale({1.f, 0.33f});
    verticalLayout->AddChild(std::move(manaBar));

    auto frame = componentContext_.guiElementFactory_.CreateGuiTexture("GUI/liberated_frame_video.png");
    window->AddChild(std::move(frame));
    window->AddChild(std::move(verticalLayout));
    guiManager_.AddLayer(DefaultGuiLayers::hud);
    guiManager_.Add(DefaultGuiLayers::hud, std::move(window));
}
void Player::Update()
{
    hudElements_.hp.update();
    hudElements_.stamina.update();
    hudElements_.mana.update();
}
void Player::hurt(int64 dmg)
{
    DEBUG_LOG("hurt 1");
    if (characterController_ and characterStatistic_.currentHp > 0)
    {
        characterStatistic_.currentHp -= dmg;
        if (characterStatistic_.currentHp > 0)
        {
            // characterController_->addState(std::make_unique<Hurt>());
        }
        else
        {
            characterController_->handleEvent(DeathEvent{});
            characterController_->Deactivate();
            Deactivate();
        }
    }
}
void Player::renderDmg(const common::Transform& enemyTransform, int64 dmg)
{
    vec2 offset(getRandomFloat() / 20.f, getRandomFloat() / 20.f);
    auto hitInfoWorldPoision   = enemyTransform.GetPosition() + vec3(0, enemyTransform.GetScale().y / 2.f, 0);
    auto hitInfoScreenPosition = componentContext_.renderersManager_.convertToScreenPosition(hitInfoWorldPoision);
    auto hitText               = componentContext_.guiElementFactory_.CreateGuiText(std::to_string(dmg));
    hitText->SetScreenScale(vec2(0.05f));
    hitText->SetScreenPostion(hitInfoScreenPosition + offset);
    hitText->SetColor(vec4(1.f, 1.f, 1.f, 0.f));

    auto hitTextPtr = hitText.get();

    guiManager_.add(GuiAnimation(
        std::move(hitText), GuiAnimation::Duration(1.f),
        [offset, hitTextPtr, &enemyTransform, &rendererManager = componentContext_.renderersManager_](
            GuiElement& text, GuiAnimation::DeltaTime deltaTime, GuiAnimation::Duration elapsedTime) mutable {
            if (elapsedTime > 0.5f)
            {
                const float speed    = 0.05f;
                auto currentPosition = text.GetScreenPosition();
                currentPosition.y += (speed * deltaTime);
                offset.y += (speed * deltaTime);
            }

            auto hitInfoWorldPoision   = enemyTransform.GetPosition() + vec3(0, enemyTransform.GetScale().y / 2.f, 0);
            auto hitInfoScreenPosition = rendererManager.convertToScreenPosition(hitInfoWorldPoision);
            text.SetScreenPostion(hitInfoScreenPosition + offset);

            auto currentColor = hitTextPtr->GetColor();
            if (elapsedTime < 0.4f)
            {
                if (currentColor.a < 1.f)
                {
                    currentColor.a += 3.f * deltaTime;
                }
            }
            else if (elapsedTime > 0.6f)
            {
                if (currentColor.a > 0.f)
                {
                    currentColor.a -= 3.f * deltaTime;
                }
            }
            hitTextPtr->SetColor(currentColor);
        }));
}
void Player::registerReadFunctions()
{
    auto readFunc = [](ComponentContext& componentContext, const TreeNode&, GameObject& gameObject) {
        return std::make_unique<Player>(componentContext, gameObject);
    };
    ReadFunctions::instance().componentsReadFunctions.insert({COMPONENT_STR, readFunc});
}
void Player::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}
}  // namespace Components
}  // namespace GameEngine
