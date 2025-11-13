#include "ComponentPickerPopup.h"

#include <GameEngine/Components/ComponentsReadFunctions.h>
#include <GameEngine/Objects/GameObject.h>
#include <Utils/TreeNode.h>

ComponentPickerPopup::ComponentPickerPopup(wxWindow* parent, GameEngine::Components::ComponentController& componentController,
                                           GameEngine::GameObject& gameObject, SelectCallback onSelect)
    : wxDialog(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE | wxDIALOG_NO_PARENT | wxSTAY_ON_TOP)
    , componentController{componentController}
    , gameObject{gameObject}
    , selectCallback(onSelect)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    searchCtrl = new wxTextCtrl(this, wxID_ANY);
    listBox    = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE | wxLB_SORT);

    sizer->Add(searchCtrl, 0, wxEXPAND | wxALL, 5);
    sizer->Add(listBox, 1, wxEXPAND | wxALL, 5);

    SetSizerAndFit(sizer);

    PopulateList("");

    searchCtrl->Bind(wxEVT_TEXT, &ComponentPickerPopup::OnSearch, this);
    listBox->Bind(wxEVT_LISTBOX, &ComponentPickerPopup::OnSelect, this);

    CallAfter([this]() { searchCtrl->SetFocus(); });
    Bind(wxEVT_CHAR_HOOK,
         [this](wxKeyEvent& evt)
         {
             if (evt.GetKeyCode() == WXK_ESCAPE)
                 Dismiss();
             else
                 evt.Skip();
         });
}

void ComponentPickerPopup::PopulateList(const wxString& filter)
{
    listBox->Freeze();
    listBox->Clear();

    for (auto [name, _] : GameEngine::Components::ReadFunctions().instance().getComponentTypeNameToId())
    {
        wxString componentName(name);
        if (filter.IsEmpty() || componentName.Lower().Contains(filter.Lower()))
            listBox->Append(componentName);
    }

    listBox->Thaw();
}

void ComponentPickerPopup::OnSearch(wxCommandEvent& evt)
{
    PopulateList(searchCtrl->GetValue());
}

void ComponentPickerPopup::OnSelect(wxCommandEvent& evt)
{
    wxString selected = listBox->GetStringSelection();

    if (GameEngine::Components::ReadFunctions().instance().isRegistered(selected.ToStdString()))
    {
        TreeNode node("component");
        node.attributes_.insert({GameEngine::Components::CSTR_TYPE, selected.ToStdString()});
        auto component = gameObject.AddComponent(node);
        if (component)
        {
            if (selectCallback)
                selectCallback(*component);
        }
        else
        {
            wxLogMessage("Read function not found for component: %s", selected);
        }
    }

    Dismiss();
}

void ComponentPickerPopup::Dismiss()
{
    Hide();
    Destroy();
}

void ComponentPickerPopup::Popup(const wxPoint& pos)
{
    Move(pos);
    Show();
    Raise();
    searchCtrl->SetFocus();
}
