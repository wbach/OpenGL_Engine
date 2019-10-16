#include "Layout.h"
#include <Logger/Log.h>
#include <algorithm>

namespace GameEngine
{
Layout::Layout(GuiElementTypes type, const vec2ui& windowSize)
    : GuiElement(type, windowSize)
    , algin_(Algin::CENTER)
{
    SetOnchangeFunction([this]() { OnChange(); });
}

Layout::~Layout()
{
}

void Layout::Update()
{
    for (auto& child : children_)
    {
        child->Update();
    }
}

void Layout::ResetView()
{
}

void Layout::SetAlgin(Algin algin)
{
    algin_ = algin;
}

Layout::Algin Layout::GetAlgin() const
{
    return algin_;
}
}  // namespace GameEngine
