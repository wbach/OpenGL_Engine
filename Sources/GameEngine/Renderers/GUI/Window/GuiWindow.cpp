#include "GuiWindow.h"

#include "Logger/Log.h"

namespace GameEngine
{
GuiElementTypes GuiWindowElement::type = GuiElementTypes::Window;

GuiWindowElement::GuiWindowElement(GuiWindowStyle style, Input::InputManager& inputManager)
    : GuiElement(GuiWindowElement::type)
    , inputManager_(inputManager)
    , style_(style)
{
    //inputSubscribtionKeyUp_ = inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [&]() { collisionPoint_ = {}; });
    //inputSubscribtionKeyDown_ = inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [&]() { CheckCollisionPoint = {}; });
}

GuiWindowElement::~GuiWindowElement()
{
    //inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, inputSubscribtionKeyDown_);
    //inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, inputSubscribtionKeyUp_);
    children_.clear();
}

void GuiWindowElement::Update()
{
    GuiElement::Update();

    if (not collisionPoint_)
        return;

    auto position = inputManager_.GetMousePosition();

    auto newPosition = position - *collisionPoint_;
    auto moveVec     = newPosition - transform_.position;

    if (glm::length(moveVec) > std::numeric_limits<float>::epsilon())
    {
        GuiElement::SetScreenPostion(newPosition);
    }
}

void GuiWindowElement::CheckCollisionPoint()
{
    auto position   = inputManager_.GetMousePosition();
    collisionPoint_ = position - transform_.position;
}
GuiWindowStyle GuiWindowElement::GetStyle() const
{
    return style_;
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
    background_->setParent(this);
    background_->SetZPosition(0.5f);
    DEBUG_LOG("Id : " + std::to_string(GetId()));
    DEBUG_LOG("BId : " + std::to_string(background_->GetId()));
    DEBUG_LOG("Screen scale " + std::to_string(GetScreenScale()));
    DEBUG_LOG("BScreen scale " + std::to_string(background_->GetScreenScale()));
    DEBUG_LOG("Screen pos " + std::to_string(GetScreenPosition()));
    DEBUG_LOG("BScreen pos " + std::to_string(background_->GetScreenPosition()));
}

void GuiWindowElement::SetBar(std::unique_ptr<GuiElement> bar)
{
    DEBUG_LOG("Not implemented");
    // bar_ = std::move(bar);
    // bar_->setParent(this);
}
}  // namespace GameEngine
