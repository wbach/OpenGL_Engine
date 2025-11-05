#include "TerrainSelectionDialog.h"

#include <GameEngine/Objects/GameObject.h>
#include <wx/clntdata.h>

#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "WxEditor/WxHelpers/WxClientData.h"

TerrainSelectionDialog::TerrainSelectionDialog(wxWindow* parent, const GameEngine::Components::ComponentController& componentController,
                                               const std::string& labelStr)
    : wxDialog(parent, wxID_ANY, "Select", wxDefaultPosition, wxSize(300, 200))
{
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(this, wxID_ANY, labelStr);
    vbox->Add(label, 0, wxALL | wxCENTER, 10);

    choice = new wxChoice(this, wxID_ANY);
    choice->Append("All terrains", new WxClientData<int>(-1));

    const auto terrains = componentController.GetAllComponentsOfType<GameEngine::Components::TerrainRendererComponent>();
    for (const auto& terrain : terrains)
    {
        const auto& parentGo = terrain->GetParentGameObject();
        choice->Append(parentGo.GetName() + "(" + std::to_string(parentGo.GetId()) + ")",
                       new WxClientData<int>(parentGo.GetId()));
    }

    vbox->Add(choice, 0, wxALL | wxEXPAND, 10);
    choice->SetSelection(0);

    wxBoxSizer* hbox       = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okButton     = new wxButton(this, wxID_OK, "Yes");
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "No");
    hbox->Add(okButton, 1, wxALL, 5);
    hbox->Add(cancelButton, 1, wxALL, 5);

    vbox->Add(hbox, 0, wxALIGN_CENTER);

    SetSizer(vbox);
    Centre();
}
wxString TerrainSelectionDialog::GetSelectionStr() const
{
    return choice->GetStringSelection();
}
int TerrainSelectionDialog::GetSelection() const
{
    return choice->GetSelection();
}
wxChoice* TerrainSelectionDialog::GetChoice()
{
    return choice;
}
