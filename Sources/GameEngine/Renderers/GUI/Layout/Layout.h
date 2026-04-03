#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/InputManager.h"

namespace GameEngine
{
class ENGINE_API Layout : public GuiElement
{
public:
    enum class Align
    {
        LEFT,
        CENTER,
        RIGHT
    };

    Layout(GuiElementTypes type);
    ~Layout();
    virtual void Update() override;
    virtual void ResetView();
    virtual void OnChange() = 0;

    void SetAlign(Align);
    Align GetAlign() const;

protected:
    Align align_;
};
}  // namespace GameEngine
