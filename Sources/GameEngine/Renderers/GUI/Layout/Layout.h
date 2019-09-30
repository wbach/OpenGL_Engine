#pragma once
#include <functional>
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
    enum class Algin
    {
        LEFT,
        CENTER,
        RIGHT
    };

    Layout(GuiElementTypes type, const vec2ui& windowSize);
    ~Layout();
    virtual void Update();
    virtual void ResetView();
    virtual LayoutElementWrapper& AddChild(std::unique_ptr<GuiElement>) = 0;
    virtual LayoutElementWrapper& AddChild(std::unique_ptr<GuiElement>, std::function<void()>);
    virtual void Remove(GuiElement*);
    virtual void Remove(uint32 id);
    virtual void RemoveAll();
    virtual void SetScale(const vec2& scale);
    virtual void Show(bool);
    virtual void Show() override;
    virtual void Hide() override;
    virtual void SetRect(const Rect& rect) override;
    virtual void SetPostion(const vec2& position) override;
    virtual void SetPostion(const vec2ui& position) override;
    virtual void SetZPosition(float z) override;
    virtual void SetZPositionOffset(float offset) override;
    virtual void SetIsInternal(bool) override;
    virtual GuiElement* GetCollisonElement(const vec2&) override;
    virtual bool CompareZValue(const GuiElement&) const override;
    virtual GuiElement* Get(const std::string& label) override;

    void SetAlgin(Algin algin);
    Algin GetAlgin() const;
    const std::vector<std::unique_ptr<LayoutElementWrapper>>& GetChildren() const;

protected:
    std::vector<std::unique_ptr<LayoutElementWrapper>> children_;
    Algin algin_;
};
}  // namespace GameEngine
