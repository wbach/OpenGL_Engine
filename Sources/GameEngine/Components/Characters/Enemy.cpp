#include "Enemy.h"

#include "GameEngine/Animations/AnimationClip.h"
#include "GameEngine/Camera/CameraWrapper.h"
#include "GameEngine/Components/Characters/Player.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/ControllerUtlis.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/GUI/GuiElementFactory.h"
#include "GameEngine/Renderers/GUI/GuiManager.h"
#include "GameEngine/Renderers/GUI/Layer/DefaultLayers.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Window/GuiWindow.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Scene/Scene.hpp"

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
    , guiManager_{componentContext.guiElementFactory_.getManager()}
    , animator_{nullptr}
    , characterController_{nullptr}
    , hud_{{characterStatistic_.currentHp, characterStatistic_.maxHp}}
{
}
void Enemy::CleanUp()
{
    if (hud_.window)
        guiManager_.Remove(*hud_.window);

    for (auto& id : animSubs_)
        animator_->UnSubscribeForAnimationFrame(id);
}
void Enemy::ReqisterFunctions()
{
    RegisterFunction(FunctionType::OnStart, [this]() { Init(); });
    RegisterFunction(FunctionType::Update, [this]() { Update(); });
}
void Enemy::Init()
{
    animator_            = thisObject_.GetComponent<Animator>();
    characterController_ = thisObject_.GetComponent<CharacterController>();

    if (animator_ and characterController_)
    {
        auto attackAction = [this]()
        {
            auto [distance, vectorToPlayer, componentPtr] = getComponentsInRange<Player>(
                componentContext_.componentController_, thisObject_.GetWorldTransform().GetPosition());

            if (componentPtr)
            {
                if (distance < (characterStatistic_.attackRange + characterController_->getShapeSize()))
                {
                    componentPtr->hurt(characterStatistic_.attackDmg);
                }
            }
        };

        for (const auto& attackName : characterController_->animationClipsNames_.armed.attack)
        {
            animSubs_.push_back(animator_->SubscribeForAnimationFrame(attackName, attackAction));
        }
        for (const auto& attackName : characterController_->animationClipsNames_.disarmed.attack)
        {
            animSubs_.push_back(animator_->SubscribeForAnimationFrame(attackName, attackAction));
        }
    }

    const vec2 windowSize(0.2f, 0.033f);
    const vec2 windowPosition(0.5f, 1.f - windowSize.y);

    auto window = componentContext_.guiElementFactory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, windowPosition,
                                                                       windowSize);
    hud_.window = window.get();
    auto verticalLayout = componentContext_.guiElementFactory_.CreateVerticalLayout();
    verticalLayout->SetAlgin(Layout::Algin::LEFT);

    auto hpRedBar   = componentContext_.guiElementFactory_.CreateGuiTexture("GUI/Health_Bars/HP/Style_1.png");
    hud_.hp.texture = hpRedBar.get();
    verticalLayout->AddChild(std::move(hpRedBar));

    auto frame = componentContext_.guiElementFactory_.CreateGuiTexture("GUI/liberated_frame_video.png");
    window->AddChild(std::move(frame));
    window->AddChild(std::move(verticalLayout));
    guiManager_.AddLayer(DefaultGuiLayers::hud);
    guiManager_.Add(DefaultGuiLayers::hud, std::move(window));
}
void Enemy::Update()
{
    hud_.hp.update();
    if (hud_.window)
    {
        auto cameraPostion  = componentContext_.scene_.GetCamera().GetPosition();
        auto toCameraVector = cameraPostion - thisObject_.GetWorldTransform().GetPosition();

        if (glm::length(toCameraVector) > 20.f)
        {
            if (hud_.window->IsShow())
                hud_.window->Hide();
        }
        else
        {
            if (not hud_.window->IsShow())
                hud_.window->Show();
        }

        return;
        // auto screenPosition =
        //    componentContext_.renderersManager_.convertToScreenPosition(thisObject_.GetWorldTransform().GetPosition());
        // hud_.window->SetScreenPostion(screenPosition);
    }
}
std::optional<int64> Enemy::hurt(int64 dmg)
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
            auto fsm = characterController_->fsm();
            if (fsm)
            {
                characterController_->fsm()->handle(DeathEvent{});
            }
            characterController_->Deactivate();
            Deactivate();
        }

        return dmg;
    }
    return std::nullopt;
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
    auto readFunc = [](ComponentContext& componentContext, const TreeNode&, GameObject& gameObject)
    { return std::make_unique<Enemy>(componentContext, gameObject); };
    ReadFunctions::instance().componentsReadFunctions.insert({COMPONENT_STR, readFunc});
}
void Enemy::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, COMPONENT_STR});
}
}  // namespace Components
}  // namespace GameEngine
