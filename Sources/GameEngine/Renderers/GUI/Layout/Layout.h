#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/InputManager.h"

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
    virtual void Update() override;
    virtual void ResetView();
    virtual void OnChange() = 0;

    void SetAlgin(Algin algin);
    Algin GetAlgin() const;

protected:
    Algin algin_;
};
}  // namespace GameEngine
