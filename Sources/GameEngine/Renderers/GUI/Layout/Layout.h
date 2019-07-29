#pragma once
#include <memory>
#include <optional>
#include <vector>
#include <functional>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/InputManager.h"
#include "LayoutElementWrapper.h"

namespace GameEngine
{
class Layout : public GuiElement
{
public:
    Layout(GuiElementTypes type, const vec2ui& windowSize, std::function< void (uint32)> unsubscribe);
    virtual LayoutElementWrapper& AddChild(GuiElement*) = 0;
    virtual void Remove(GuiElement*);
    virtual void Remove(uint32 id);
    virtual void RemoveAll();
    virtual void SetScale(const vec2& scale);
    virtual void Show(bool);
    virtual void Show() override;
    virtual void Hide() override;

protected:
    std::vector<LayoutElementWrapper> children_;
    std::vector<GuiElement*> elements_;
    std::function< void (uint32)> unsubscribe_;
};
}  // namespace GameEngine
