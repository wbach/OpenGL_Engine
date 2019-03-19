#pragma once
#include "IGuiElement.h"
#include "Rect.h"
#include <memory>

namespace GameEngine
{
class GuiElement : public IGuiElement
{
protected:
    Rect rect_;
    bool show_ = true;
    std::vector<std::unique_ptr<IGuiElement>> children_;
};
}  // namespace GameEngine
