
#include "GuiWindow.h"
#include "Logger/Log.h"

namespace GameEngine
{
GuiElementTypes GuiWindowElement::type = GuiElementTypes::Window;

GuiWindowElement::GuiWindowElement(GuiWindowStyle style, Input::InputManager& inputManager)
    : GuiElement(GuiWindowElement::type)
    , inputManager_(inputManager)
    , titleBarSize_(0.43f)
    , style_(style)
{
    inputSubscribtionKeyUp_ = inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [&]() { collisionPoint_ = {}; });
}

GuiWindowElement::~GuiWindowElement()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, inputSubscribtionKeyDown_);
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, inputSubscribtionKeyUp_);
    children_.clear();
}

void GuiWindowElement::Update()
{
    GuiElement::Update();

    if (not collisionPoint_)
        return;

    auto position = inputManager_.GetMousePosition();

    auto newPosition = position - *collisionPoint_;
    auto moveVec     = newPosition - position_;

    if (glm::length(moveVec) > std::numeric_limits<float>::min())
    {
        GuiElement::SetPostion(newPosition);
    }
}

void GuiWindowElement::CheckCollisionPoint()
{
    auto position   = inputManager_.GetMousePosition();
    collisionPoint_ = position - position_;
}
GuiWindowStyle GuiWindowElement::GetStyle() const
{
    return style_;
}

void GuiWindowElement::SetScale(const vec2& scale)
{
    GuiElement::SetScale(scale);

    if (background_)
    {
        background_->SetScale(scale);
    }

    if (bar_)
    {
        auto barScale = bar_->GetScale();
        barScale.x    = scale.x;
        bar_->SetScale(barScale);
        const vec2 barPosition(0, GetScale().y + GUI_WINDOW_BAR_HEIGHT);
        bar_->SetPostion(barPosition);
    }
}

void GuiWindowElement::SetPostion(const vec2& position)
{
    GuiElement::SetPostion(position);

    if (background_)
    {
        background_->SetPostion(position);
    }

    if (bar_)
    {
        bar_->SetPostion(position);
    }
}

void GuiWindowElement::Show(bool v)
{
    GuiElement::Show(v);

    if (background_)
    {
        background_->Show(v);
    }

    if (bar_)
    {
        bar_->Show(v);
    }
}

void GuiWindowElement::Show()
{
    GuiElement::Show();

    if (background_)
    {
        background_->Show();
    }

    if (bar_)
    {
        bar_->Show();
    }
}

void GuiWindowElement::Hide()
{
    GuiElement::Hide();

    if (background_)
    {
        background_->Hide();
    }

    if (bar_)
    {
        bar_->Hide();
    }
}

void GuiWindowElement::SetBackground(std::unique_ptr<GuiElement> background)
{
    background_ = std::move(background);
    background_->SetPostion(position_);
    background_->SetScale(scale_);
    background_->SetZPosition(0.5f);
}

void GuiWindowElement::SetBar(std::unique_ptr<GuiElement> bar)
{
    bar_ = std::move(bar);
}
}  // namespace GameEngine
