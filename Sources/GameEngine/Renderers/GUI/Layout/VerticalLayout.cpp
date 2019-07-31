#include "VerticalLayout.h"

namespace GameEngine
{
GuiElementTypes VerticalLayout::type = GuiElementTypes::VerticalLayout;

VerticalLayout::VerticalLayout(const vec2ui &windowSize, Input::InputManager &inputManager, std::function<void(uint32)> unsubscribe)
    : Layout(type, windowSize, unsubscribe)
    , inputManager_(inputManager)
{
}

LayoutElementWrapper &VerticalLayout::AddChild(GuiElement *element)
{
    children_.emplace_back(*element, [this]() { OnChange(); });
    elements_.push_back(element);
    OnChange();
    return children_.back();
}

void VerticalLayout::OnChange()
{
    if (children_.empty())
        return;

    vec2 newPosition = position_;
    newPosition.y += scale_.y;

    elements_[0]->SetPostion(newPosition);

    for (std::size_t i = 1; i < elements_.size(); ++i)
    {
        const auto &position = elements_[i - 1]->GetPosition().y;
        const auto &scale    = elements_[i - 1]->GetScale().y;

        newPosition.y = position - 2.f * scale;
        elements_[i]->SetPostion(newPosition);
    }
}

}  // namespace GameEngine
