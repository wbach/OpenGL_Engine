#include "ComponentPickerPopup.h"

wxBEGIN_EVENT_TABLE(ComponentPickerPopup, wxPopupTransientWindow)
    // eventy bindowane dynamicznie
wxEND_EVENT_TABLE()

ComponentPickerPopup::ComponentPickerPopup(wxWindow* parent,
                                           const std::vector<wxString>& components,
                                           SelectCallback onSelect)
    : wxPopupTransientWindow(parent, wxBORDER_SIMPLE),
      allComponents(components),
      selectCallback(onSelect)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    searchCtrl = new wxTextCtrl(this, wxID_ANY);
    listBox = new wxListBox(this, wxID_ANY);

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

    for (const auto& comp : allComponents)
    {
        if (filter.IsEmpty() || comp.Lower().Contains(filter.Lower()))
            listBox->Append(comp);
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
    if (selectCallback)
        selectCallback(selected);

    Dismiss(); // zamykamy popup
}

void ComponentPickerPopup::OnDismiss()
{
    // Tu możesz coś zrobić po zamknięciu popupu (np. zwolnić zasoby)
}
