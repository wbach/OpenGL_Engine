#pragma once
#include <wx/wx.h>

namespace GameEngine::Components
{
class ComponentController;
}

class TerrainSelectionDialog : public wxDialog
{
public:
    TerrainSelectionDialog(wxWindow*, const GameEngine::Components::ComponentController&, const std::string&);

    wxString GetSelectionStr() const;
    int GetSelection() const;
    wxChoice* GetChoice();

private:
    wxChoice* choice;
};