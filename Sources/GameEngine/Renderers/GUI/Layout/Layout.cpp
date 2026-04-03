#include "Layout.h"
#include <Logger/Log.h>
#include <algorithm>

namespace GameEngine
{
Layout::Layout(GuiElementTypes type)
    : GuiElement(type)
    , align_(Align::CENTER)
{
    SubscribeForChange([this]() { OnChange(); });
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

void Layout::SetAlign(Align align)
{
    align_ = align;
}

Layout::Align Layout::GetAlign() const
{
    return align_;
}
}  // namespace GameEngine
