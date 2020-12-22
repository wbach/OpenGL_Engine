#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/InputManager.h"

namespace GameEngine
{
class ContextMenu : public GuiElement
{
public:
    ContextMenu();
    virtual void Update() override;

private:
    static GuiElementTypes type;
};
}  // namespace GameEngine
