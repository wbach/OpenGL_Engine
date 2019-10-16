
#include "GuiWindow.h"
#include "Logger/Log.h"

namespace GameEngine
{
GuiElementTypes GuiWindowElement::type = GuiElementTypes::Window;

GuiWindowElement::GuiWindowElement(GuiWindowStyle style, const vec2ui& windowSize, Input::InputManager& inputManager)
    : GuiElement(GuiWindowElement::type, windowSize)
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
}  // namespace GameEngine
