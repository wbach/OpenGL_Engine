#pragma once
#include "Layout.h"
namespace GameEngine
{
namespace GUI
{
class ENGINE_API HorizontalLayout : public Layout
{
public:
    HorizontalLayout();
    void autoResize();
    std::unique_ptr<Element> clone() const override;

private:
    float calculateFirstChildXPosition() const;
    float calculateYPosition(const Element &);
    void refreshSelf() override;
    void calculateTotalChildrenScale();
    void accept(IElementVisitor &) override;

private:
    float totalChildrenScaleX_;
    bool resizeAble_;
};
}  // namespace GUI
}  // namespace GameEngine
