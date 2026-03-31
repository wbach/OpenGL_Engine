#pragma once
#include <wx/treectrl.h>

namespace GameEngine
{
class GuiElement;
class GuiLayer;
}  // namespace GameEngine

class GuiTreeItemData : public wxTreeItemData
{
public:
    GameEngine::GuiElement* element{nullptr};
    GameEngine::GuiLayer* layer{nullptr};
    GuiTreeItemData(GameEngine::GuiElement* e)
        : element(e)
    {
    }
    GuiTreeItemData(GameEngine::GuiLayer* e)
        : layer(e)
    {
    }
};
