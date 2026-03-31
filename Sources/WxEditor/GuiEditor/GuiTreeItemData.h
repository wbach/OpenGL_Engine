#pragma once
#include <wx/treectrl.h>

namespace GameEngine
{
class GuiElement;
}  // namespace GameEngine

class GuiTreeItemData : public wxTreeItemData
{
public:
    GameEngine::GuiElement* element{nullptr};
    GuiTreeItemData(GameEngine::GuiElement* e)
        : element(e)
    {
    }
};
