#pragma once
#include "GameEngine/Renderers/GUI/Element.h"
#include "GameEngine/Renderers/GUI/HorizontalAlign.h"
#include "GameEngine/Renderers/GUI/VerticalAlign.h"
namespace GameEngine
{
namespace GUI
{
class ENGINE_API Layout : public Element
{
public:
    Layout();
    ~Layout() override;

    virtual void resetView();

    void setAlign(HorizontalAlign);
    void setAlign(VerticalAlign);

    HorizontalAlign getHorizontalAlign() const;
    VerticalAlign getVerticalAlign() const;

protected:
    HorizontalAlign hAlign_;
    VerticalAlign vAlign_;
};
}  // namespace GUI
}  // namespace GameEngine
