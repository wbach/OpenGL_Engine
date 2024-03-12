#include "HorizontalLayout.h"

#include <Logger/Log.h>

namespace GameEngine
{
GuiElementTypes HorizontalLayout::type = GuiElementTypes::HorizontalLayout;

HorizontalLayout::HorizontalLayout()
    : Layout(type)
    , totalChildrenScaleX_{0}
    , resizeAble_{false}
{
}

void HorizontalLayout::AddChild(std::unique_ptr<GuiElement> child)
{
    if (resizeAble_)
    {
        if (transform_.scale.y < child->GetLocalScale().y)
        {
            SetLocalScale({transform_.scale.x, child->GetLocalScale().y});
        }
    }

    GuiElement::AddChild(std::move(child));
}

void HorizontalLayout::autoResize()
{
    resizeAble_      = true;
    transform_.scale = {0, 0};

    for (auto &child : children_)
    {
        if (transform_.scale.y < child->GetLocalScale().y)
        {
            SetLocalScale({transform_.scale.x, child->GetLocalScale().y});
        }
    }
}

float HorizontalLayout::calculateFirstChildXPosition() const
{
    if (not resizeAble_)
    {
        if (algin_ == Algin::LEFT)
        {
            return children_.front()->GetLocalScale().x / 2.f;
        }

        if (algin_ == Algin::RIGHT)
        {
            return 1.f -totalChildrenScaleX_ + children_.front()->GetLocalScale().x / 2.f;
        }
    }
    return 0.5f - (totalChildrenScaleX_ / 2.f) + children_.front()->GetLocalScale().x / 2.f;
}

void HorizontalLayout::OnChange()
{
    if (children_.empty())
        return;

    DisableChangeNotif();
    calculateTotalChildrenScaleX();

    children_.front()->SetLocalPostion({calculateFirstChildXPosition(), 0});

    for (std::size_t i = 1; i < children_.size(); ++i)
    {
        const auto &parent = *children_[i - 1];
        auto &current      = *children_[i];

        auto posX = parent.GetLocalPosition().x + (parent.GetLocalScale().x / 2.f) + (current.GetLocalScale().x / 2.f);
        current.SetLocalPostion({posX, 0.f});
    }

    EnableChangeNotif();
}

void GameEngine::HorizontalLayout::calculateTotalChildrenScaleX()
{
    totalChildrenScaleX_ = 0.f;
    for (auto &child : children_)
    {
        totalChildrenScaleX_ += child->GetLocalScale().x;
    }

    if (resizeAble_)
    {
        SetLocalScale({totalChildrenScaleX_, GetLocalScale().y});
    }
}
}  // namespace GameEngine
