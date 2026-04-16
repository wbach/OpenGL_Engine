#include "HorizontalLayout.h"

#include <Logger/Log.h>

namespace GameEngine
{
namespace GUI
{
HorizontalLayout::HorizontalLayout()
    : totalChildrenScaleX_{0}
    , resizeAble_{false}
{
}

void HorizontalLayout::autoResize()
{
    resizeAble_     = true;
    transform.scale = {0, 0};

    for (auto &child : children)
    {
        if (transform.scale.y < child->getLocalScale().y)
        {
            transform.scale = {transform.scale.x, child->getLocalScale().y};
        }
    }
}

float HorizontalLayout::calculateFirstChildXPosition() const
{
    auto &first          = *children.front();
    auto firstHalfWidth  = first.getLocalScale().x / 2.f;
    auto firstMarginLeft = first.getMargin().left;

    if (not resizeAble_)
    {
        if (hAlign_ == HorizontalAlign::LEFT)
        {
            return 0.f + padding_.left + firstMarginLeft + firstHalfWidth;
        }

        if (hAlign_ == HorizontalAlign::RIGHT)
        {
            return 1.f - totalChildrenScaleX_ + padding_.left + firstMarginLeft + firstHalfWidth;
        }
    }

    return (0.5f - (totalChildrenScaleX_ / 2.f)) + padding_.left + firstMarginLeft + firstHalfWidth;
}

void HorizontalLayout::refreshSelf()
{
    if (children.empty())
        return;

    calculateTotalChildrenScale();

    auto &firstChild = children.front();
    firstChild->setInternalPosition({calculateFirstChildXPosition(), calculateYPosition(*firstChild)});

    for (std::size_t i = 1; i < children.size(); ++i)
    {
        const auto &parent = *children[i - 1];
        auto &current      = *children[i];

        auto gap = (parent.getLocalScale().x / 2.f) + parent.getMargin().right + current.getMargin().left +
                   (current.getLocalScale().x / 2.f);

        auto posX = parent.getLocalPosition().x + gap;
        current.setInternalPosition({posX, calculateYPosition(current)});
    }
}

void HorizontalLayout::calculateTotalChildrenScale()
{
    float newTotalX = padding_.left + padding_.right;
    float maxHeight = 0.f;

    for (auto &child : children)
    {
        const auto &margin = child->getMargin();
        newTotalX += child->getLocalScale().x + margin.left + margin.right;

        auto childReservedHeight = child->getLocalScale().y + margin.top + margin.bottom;
        if (childReservedHeight > maxHeight)
        {
            maxHeight = child->getLocalScale().y;
        }
    }

    totalChildrenScaleX_ = newTotalX;
    maxHeight += padding_.top + padding_.bottom;

    if (resizeAble_)
    {
        if (std::abs(transform.scale.x - newTotalX) > 0.0001f or std::abs(transform.scale.y - maxHeight) > 0.0001f)
        {
            transform.scale = {newTotalX, maxHeight};
            if (parent)
                parent->invalidate();
        }
    }
}
void HorizontalLayout::accept(IElementVisitor &visitor)
{
    visitor.visit(*this);
}
float HorizontalLayout::calculateYPosition(const Element &element)
{
    auto childHeight   = element.getLocalScale().y;
    const auto &margin = element.getMargin();

    if (vAlign_ == VerticalAlign::TOP)
    {
        return 1.0f - padding_.top - margin.top - (childHeight / 2.0f);
    }
    else if (vAlign_ == VerticalAlign::BOTTOM)
    {
        return 0.0f + padding_.bottom + margin.bottom + (childHeight / 2.0f);
    }

    return 0.5f + ((padding_.bottom + margin.bottom) - (padding_.top + margin.top)) / 2.f;
}
std::unique_ptr<Element> HorizontalLayout::clone() const
{
    return std::make_unique<HorizontalLayout>(*this);
}
}  // namespace GUI
}  // namespace GameEngine
