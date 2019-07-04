#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/InputManager.h"
#include "LayoutElementWrapper.h"

namespace GameEngine
{
class Layout : public GuiElement
{
public:
    Layout(GuiElementTypes type, const vec2ui& windowSize);
    virtual LayoutElementWrapper& AddChild(GuiElement*) = 0;
    virtual void Remove(GuiElement*);
    virtual void Remove(uint32 id);
    virtual void SetScale(const vec2& scale);
    virtual void Show(bool);
    virtual void Show() override;
    virtual void Hide() override;

protected:
    std::vector<LayoutElementWrapper> children_;
    std::vector<GuiElement*> elements_;
};
}  // namespace GameEngine
