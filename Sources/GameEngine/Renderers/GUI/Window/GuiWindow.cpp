#include "GuiWindow.h"
#include "Logger/Log.h"

namespace GameEngine
{
template <class T>
void UpdateChildPosition(GuiElement& element, const T& v)
{
    auto rect = element.GetRect();
    rect.position.x += v.x;
    rect.position.y += v.y;
    element.SetRect(rect);
}

template <class T>
void SetChildPosition(GuiElement& element, const T& v)
{
    auto rect       = element.GetRect();
    rect.position.x = v.x;
    rect.position.y = v.y;
    element.SetRect(rect);
}

GuiElementTypes GuiWindowElement::type = GuiElementTypes::Window;

GuiWindowElement::GuiWindowElement(const vec2ui& windowSize, Input::InputManager& inputManager)
    : GuiElement(type, windowSize)
    , inputManager_(inputManager)
{
}

void GuiWindowElement::AddChild(std::unique_ptr<GuiElement> element)
{
    UpdateChildPosition(*element, rect_.position);
    children_.push_back(std::move(element));

    inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [&]() {
        auto position   = inputManager_.GetMousePosition();
        collisionPoint_ = GetCollisionPoint(position);
    });

    inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [&]() { collisionPoint_ = {}; });
}

void GuiWindowElement::Update()
{
    if (not collisionPoint_)
        return;

    auto position = inputManager_.GetMousePosition();

    auto newPosition = position + *collisionPoint_;
    SetPostion(newPosition);
    for (auto& child : children_)
    {
        child->SetPostion(newPosition);
    }
}

}  // namespace GameEngine
