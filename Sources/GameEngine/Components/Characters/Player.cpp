#include "Player.h"

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Components/Characters/Enemy.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/ControllerUtlis.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/GUI/Layer/DefaultLayers.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"

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

    const vec2 windowSize(0.2f, 0.1f);
    const vec2 windowPosition(0.01f);

    auto window         = componentContext_.guiElementFactory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY,
                                                                       windowPosition + 0.5f * windowSize, windowSize);
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
            characterController_->addState(std::make_unique<Hurt>());
        }
        else
        {
            characterController_->addState(std::make_unique<Death>());
            characterController_->Deactivate();
        }
    }
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
void Player::HudElements::Bar::update()
{
    if (texture and (currentRendered != current or maxRendered != maxValue))
    {
        maxRendered     = maxValue;
        currentRendered = current;

        auto p = static_cast<float>(current) / static_cast<float>(maxValue);
        texture->SetLocalScale(vec2(p, texture->GetLocalScale().y));
    }
}
}  // namespace Components
}  // namespace GameEngine
