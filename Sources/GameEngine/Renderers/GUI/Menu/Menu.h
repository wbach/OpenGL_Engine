#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GameEngine/Renderers/GUI/Layout/HorizontalLayout.h"
#include "Input/InputManager.h"

namespace GameEngine
{
class GuiElementFactory;
class GuiButtonElement;
class HorizontalLayout;
class VerticalLayout;

class Menu : public GuiElement
{
public:
    Menu(GuiElementFactory&);
    virtual void Update() override;

    void AddRootElement(const std::string&);
    void Add(const std::string& parentLabel, const std::string& text, const std::function<void(GuiElement&)>);

private:
    void HideAllMenus();

private:
    static GuiElementTypes type;
    const std::string rootLabel_;
    const vec2 rootScale_;
    const vec2 rootPosition_;
    GuiElementFactory& factory_;
    HorizontalLayout* rootLayout_;
    std::unordered_map<std::string, VerticalLayout*> roots_;
};
}  // namespace GameEngine
