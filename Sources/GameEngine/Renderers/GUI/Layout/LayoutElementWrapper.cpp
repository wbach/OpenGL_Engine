#include "LayoutElementWrapper.h"
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace GameEngine
{
LayoutElementWrapper::LayoutElementWrapper(GuiElement &element, std::function<void()> onChange)
    : element_(element)
    , onChange_(onChange)
{
}

LayoutElementWrapper::LayoutElementWrapper(const LayoutElementWrapper &wrapper)
    : LayoutElementWrapper(wrapper.element_, wrapper.onChange_)
{
}

void LayoutElementWrapper::SetRect(const Rect &rect)
{
    element_.SetRect(rect);
    onChange_();
}

void LayoutElementWrapper::SetSize(const vec2ui &size)
{
    element_.SetSize(size);
    onChange_();
}

void LayoutElementWrapper::SetScale(const vec2 &scale)
{
    element_.SetScale(scale);
    onChange_();
}

void LayoutElementWrapper::SetPostion(const vec2 &position)
{
    element_.SetPostion(position);
    onChange_();
}

void LayoutElementWrapper::SetPostion(const vec2ui &position)
{
    element_.SetPostion(position);
    onChange_();
}

void LayoutElementWrapper::SetPositionWithoutNotif(const vec2 &position)
{
    element_.SetPostion(position);
}

void LayoutElementWrapper::SetZPosition(float z)
{
    element_.SetZPosition(z);
    onChange_();
}

void LayoutElementWrapper::SetZPositionOffset(float offset)
{
    element_.SetZPositionOffset(offset);
    onChange_();
}

void LayoutElementWrapper::Rotate(float r)
{
    element_.Rotate(r);
    onChange_();
}

void LayoutElementWrapper::Show(bool b)
{
    element_.Show(b);
    onChange_();
}

void LayoutElementWrapper::Show()
{
    element_.Show();
    onChange_();
}

void LayoutElementWrapper::ShowWithoutNotif()
{
    element_.Show();
}

void LayoutElementWrapper::Hide()
{
    element_.Hide();
    onChange_();
}

void LayoutElementWrapper::HideWithoutNotif()
{
    element_.Hide();
}

uint32 LayoutElementWrapper::GetId() const
{
    return element_.GetId();
}

const GuiElement &LayoutElementWrapper::Get() const
{
    return element_;
}

LayoutElementWrapper LayoutElementWrapper::operator=(const LayoutElementWrapper &)
{
    return LayoutElementWrapper(element_, onChange_);
}

void LayoutElementWrapper::MarkToRemove()
{
    element_.MarkToRemove();
}

}  // namespace GameEngine
