#include "Layout.h"

#include "GameEngine/Renderers/GUI/VerticalAlign.h"

namespace GameEngine
{
namespace GUI
{
Layout::Layout()
    : hAlign_(HorizontalAlign::CENTER)
    , vAlign_(VerticalAlign::CENTER)
{
}
Layout::~Layout()
{
}
void Layout::resetView()
{
}
void Layout::setAlign(HorizontalAlign align)
{
    hAlign_ = align;
    invalidate();
}
HorizontalAlign Layout::getHorizontalAlign() const
{
    return hAlign_;
}
void Layout::setAlign(VerticalAlign v)
{
    vAlign_ = v;
    invalidate();
}
VerticalAlign Layout::getVerticalAlign() const
{
    return vAlign_;
}
}  // namespace GUI
}  // namespace GameEngine
