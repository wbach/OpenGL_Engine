#pragma once
#include <memory>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace GameEngine
{
class GuiLayer
{
public:
    GuiLayer();
    GuiLayer(const std::string&);

    void Show();
    void Hide();
    void Add(std::unique_ptr<GuiElement>);
    std::vector<std::unique_ptr<GuiElement>>& GetElements();
    const std::vector<std::unique_ptr<GuiElement>>& GetElements() const;
    const std::string& GetName() const;
    void SetZPosition(float z);

private:
    std::string name;
    std::vector<std::unique_ptr<GuiElement>> children_;

private:
    static uint32 LAYERID;
};
}  // namespace GameEngine
