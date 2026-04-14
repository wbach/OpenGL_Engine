#include "VerticalLayout.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>
namespace GameEngine
{
namespace GUI
{
VerticalLayout::VerticalLayout()
    : viewPosition_(0.f)
    , scrollSensitive_(0.02f)
    , adjustSize_{false}

{
}
VerticalLayout::~VerticalLayout()
{
}
void VerticalLayout::resetView()
{
    viewPosition_ = 0.f;
}
void VerticalLayout::refreshSelf()
{
    if (children.empty() or not isActive())
    {
        if (adjustSize_)
            transform.scale.y = 0;
        return;
    }

    adjustSize();
    auto &firstChild = *children[0];

    vec2 newPosition{};
    newPosition.x = calculateXPosition(firstChild);
    newPosition.y = calculateFirstChildYPosition();
    firstChild.setInternalPosition(newPosition);

    for (std::size_t i = 1; i < children.size(); ++i)
    {
        const auto &parent = *children[i - 1];
        auto &child        = *children[i];

        const auto &parentPositionY = parent.getLocalPosition().y;
        const auto &parentScaleY    = parent.getLocalScale().y;

        newPosition.x = calculateXPosition(child);
        newPosition.y = parentPositionY - (parentScaleY / 2.f) - ((child.getLocalScale().y / 2.f));
        child.setInternalPosition(newPosition);
    }

    updateVisibility();
}
float VerticalLayout::calculateXPosition(const Element &element)
{
    float result = 0.5f;

    if (hAlign_ == HorizontalAlign::LEFT)
    {
        result -= (1.f - element.getLocalScale().x) / 2.f;
    }
    else if (hAlign_ == HorizontalAlign::RIGHT)
    {
        result += (1.f - element.getLocalScale().x) / 2.f;
    }

    return result;
}
void VerticalLayout::updateVisibility()
{
    for (auto &element : children)
    {
        element->activateInternal(isVisible(*element));
    }
}
void VerticalLayout::adjustSize()
{
    if (not adjustSize_ or children.empty())
        return;

    float newHeight = calculateTotalChildrenHeight();

    if (std::abs(transform.scale.y - newHeight) > 0.0001f)
    {
        transform.scale.y = newHeight;
        if (parent)
            parent->invalidate();
    }
}
bool VerticalLayout::isVisible(const Element &child) const
{
    bool bottomBorder = child.getLocalPosition().y - (child.getLocalScale().y / 2.f) < (-std::numeric_limits<float>::epsilon());
    bool upperBorder =
        child.getLocalPosition().y + (child.getLocalScale().y / 2.f) > (1.f + std::numeric_limits<float>::epsilon());

    return (not autoHideElements_) or not(bottomBorder or upperBorder);
}
void VerticalLayout::enableFixedSize()
{
    adjustSize_ = true;
}
void VerticalLayout::disableFixedSize()
{
    adjustSize_ = false;
}
void VerticalLayout::autoHideElements(bool v)
{
    autoHideElements_ = v;
    updateVisibility();
}
bool VerticalLayout::autoHideElements() const
{
    return autoHideElements_;
}
void VerticalLayout::accept(IElementVisitor &visitor)
{
    visitor.visit(*this);
}
float VerticalLayout::calculateFirstChildYPosition() const
{
    float totalHeight = calculateTotalChildrenHeight();

    if (not adjustSize_)
    {
        if (vAlign_ == VerticalAlign::TOP)
        {
            return 1.f - (children.front()->getLocalScale().y / 2.f) - viewPosition_;
        }
        if (vAlign_ == VerticalAlign::BOTTOM)
        {
            return totalHeight - (children.front()->getLocalScale().y / 2.f) - viewPosition_;
        }
        if (vAlign_ == VerticalAlign::CENTER)
        {
            return 0.5f + (totalHeight / 2.f) - (children.front()->getLocalScale().y / 2.f) - viewPosition_;
        }
    }

    return 1.f - (children.front()->getLocalScale().y / 2.f) - viewPosition_;
}
float VerticalLayout::calculateTotalChildrenHeight() const
{
    float totalHeight = 0.f;
    for (const auto &child : children)
    {
        totalHeight += child->getLocalScale().y;
    }
    return totalHeight;
}
bool VerticalLayout::onMouseWheel(const vec2 &delta)
{
    if (not isActive() or adjustSize_)
    {
        return Element::onMouseWheel(delta);
    }

    if (children.empty())
    {
        return true;
    }

    if (delta.y > 0.f)
    {
        if (viewPosition_ < 0.f)
        {
            viewPosition_ += scrollSensitive_;
            invalidate();
        }
    }
    else if (delta.y < 0.f)
    {
        bool isLastVisible = children.back()->isActive();
        if (!isLastVisible)
        {
            viewPosition_ -= scrollSensitive_;
            invalidate();
        }
    }

    return true;
}
}  // namespace GUI
}  // namespace GameEngine
