#pragma once
#include "Layout.h"
namespace GameEngine
{
namespace GUI
{
class ENGINE_API VerticalLayout : public Layout
{
public:
    VerticalLayout();
    ~VerticalLayout() override;

    void resetView() override;
    bool onMouseWheel(const vec2&) override;
    std::unique_ptr<Element> clone() const override;
    void enableFixedSize();
    void disableFixedSize();
    void autoHideElements(bool);
    bool autoHideElements() const;

private:
    void refreshSelf() override;
    float calculateXPosition(const Element&);
    void updateVisibility();
    void adjustSize();
    bool isVisible(const Element&) const;
    void accept(IElementVisitor&) override;
    float calculateFirstChildYPosition() const;
    float calculateTotalChildrenHeight() const;

private:
    float viewPosition_;
    float scrollSensitive_;
    bool adjustSize_;
    bool autoHideElements_{true};

    std::vector<uint32> hiddenELements_;
};
}  // namespace GUI
}  // namespace GameEngine
