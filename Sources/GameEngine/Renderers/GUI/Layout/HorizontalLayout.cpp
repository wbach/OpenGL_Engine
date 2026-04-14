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
    if (not resizeAble_)
    {
        if (hAlign_ == HorizontalAlign::LEFT)
        {
            return children.front()->getLocalScale().x / 2.f;
        }

        if (hAlign_ == HorizontalAlign::RIGHT)
        {
            return 1.f - totalChildrenScaleX_ + children.front()->getLocalScale().x / 2.f;
        }
    }
    return 0.5f - (totalChildrenScaleX_ / 2.f) + children.front()->getLocalScale().x / 2.f;
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

        auto posX = parent.getLocalPosition().x + (parent.getLocalScale().x / 2.f) + (current.getLocalScale().x / 2.f);
        current.setInternalPosition({posX, calculateYPosition(current)});
    }
}

void HorizontalLayout::calculateTotalChildrenScale()
{
    float newTotalX = 0.f;
    float maxHeight = 0.f;

    for (auto &child : children)
    {
        newTotalX += child->getLocalScale().x;
        if (child->getLocalScale().y > maxHeight)
        {
            maxHeight = child->getLocalScale().y;
        }
    }

    totalChildrenScaleX_ = newTotalX;

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
    float childHeight = element.getLocalScale().y;

    if (vAlign_ == VerticalAlign::TOP)
    {
        return 1.0f - (childHeight / 2.0f);
    }
    else if (vAlign_ == VerticalAlign::BOTTOM)
    {
        return 0.0f + (childHeight / 2.0f);
    }

    return 0.5f;
}
std::unique_ptr<Element> HorizontalLayout::clone() const
{
    return std::make_unique<HorizontalLayout>(*this);
}
}  // namespace GUI
}  // namespace GameEngine
