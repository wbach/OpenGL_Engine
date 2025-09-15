#include "ComponentPickerPopup.h"

#include <GameEngine/Components/ComponentsReadFunctions.h>
#include <GameEngine/Objects/GameObject.h>

// clang-format off
wxBEGIN_EVENT_TABLE(ComponentPickerPopup, wxPopupTransientWindow)
    // eventy bindowane dynamicznie
wxEND_EVENT_TABLE()

ComponentPickerPopup::ComponentPickerPopup(wxWindow* parent, GameEngine::Components::ComponentController& componentController, GameEngine::GameObject& gameObject, SelectCallback onSelect)
    : wxPopupTransientWindow(parent, wxBORDER_SIMPLE)
    , componentController{componentController}
    , gameObject{gameObject}
    , selectCallback(onSelect)
{
    // clang-format on
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    searchCtrl = new wxTextCtrl(this, wxID_ANY);
    listBox    = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE | wxLB_SORT);

    sizer->Add(searchCtrl, 0, wxEXPAND | wxALL, 5);
    sizer->Add(listBox, 1, wxEXPAND | wxALL, 5);

    SetSizerAndFit(sizer);

    PopulateList("");

    searchCtrl->Bind(wxEVT_TEXT, &ComponentPickerPopup::OnSearch, this);
    listBox->Bind(wxEVT_LISTBOX, &ComponentPickerPopup::OnSelect, this);
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
    wxString val = searchCtrl->GetValue();
    PopulateList(val);
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
            component->ReqisterFunctions();
            auto maybeAwakeFunc = component->getRegisteredFunctionId(GameEngine::Components::FunctionType::Awake);
            if (maybeAwakeFunc)
            {
                componentController.callComponentFunction(gameObject.GetId(), GameEngine::Components::FunctionType::Awake,
                                                          *maybeAwakeFunc);
            }

            if (selectCallback)
                selectCallback(*component);
        }
        else
        {
            wxLogMessage("Read function not found for component: %s", selected);
        }
    }

    Dismiss();  // zamykamy popup
}

void ComponentPickerPopup::OnDismiss()
{
    // Tu mozesz cos zrobic po zamknieciu popupu (np. zwolnic zasoby)
}
