#pragma once
#include <wx/treectrl.h>

namespace GameEngine::GUI
{
class Element;
}  // namespace GameEngine::GUI

class GuiTreeItemData : public wxTreeItemData
{
public:
    GameEngine::GUI::Element* element{nullptr};
    GuiTreeItemData(GameEngine::GUI::Element* e)
        : element(e)
    {
    }
};
