#pragma once
#include <wx/wx.h>

namespace GameEngine::Components
{
class ComponentController;
}

class TerrainSelectionDialog : public wxDialog
{
public:
    TerrainSelectionDialog(wxWindow*, GameEngine::Components::ComponentController&, const std::string&);

    wxString GetSelectionStr() const;
    auto GetSelection() const;
    wxChoice* GetChoice();

private:
    wxChoice* choice;
};