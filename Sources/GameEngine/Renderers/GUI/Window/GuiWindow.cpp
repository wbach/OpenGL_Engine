#include "GuiWindow.h"
#include "Logger/Log.h"

namespace GameEngine
{

void UpdatePosition(GuiElement& element, const vec2ui& v)
{
    auto rect = element.GetRect();
    rect.position.x += v.x;
    rect.position.y += v.y;
    element.SetRect(rect);
}

void UpdatePosition(GuiElement& element, const vec2& v)
{
    auto position = element.GetPosition();
    position = position + v;
    element.SetPostion(position);
}


void SetPosition(GuiElement& element, const vec2ui& v)
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
    , titleBarSize_(0.43f)
{
}

void GuiWindowElement::AddChild(GuiElement* element)
{
    UpdatePosition(*element, rect_.position);
    children_.push_back(element);

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

    auto barYStart = scale_.y - (scale_.y * titleBarSize_);

    if (collisionPoint_->y > barYStart)
    {
        auto newPosition = position  - *collisionPoint_;
        auto moveVec =  newPosition - position_;

        SetPostion(newPosition);
        for (auto& child : children_)
        {
            UpdatePosition(*child, moveVec);
        }
    }
    else
    {
       
        // Is on top
    }
}

}  // namespace GameEngine
