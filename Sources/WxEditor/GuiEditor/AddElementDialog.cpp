#include "AddElementDialog.h"

AddElementDialog::AddElementDialog(wxWindow* parent, const std::vector<std::string>& layers)
    : wxDialog(parent, wxID_ANY, "Add Element", wxDefaultPosition, wxSize(300, 200))
{
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(new wxStaticText(this, wxID_ANY, "Select Target Layer:"), 0, wxALL, 10);

    auto* comboSizer = new wxBoxSizer(wxHORIZONTAL);
    layerCombo       = new wxComboBox(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);

    for (const auto& l : layers)
        layerCombo->Append(l);
    if (layerCombo->GetCount() > 0)
        layerCombo->SetSelection(0);

    auto* addLayerBtn = new wxButton(this, wxID_ANY, "+", wxDefaultPosition, wxSize(30, -1));

    comboSizer->Add(layerCombo, 1, wxEXPAND | wxRIGHT, 5);
    comboSizer->Add(addLayerBtn, 0);
    mainSizer->Add(comboSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    mainSizer->AddStretchSpacer();
    mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER | wxALL, 10);

    SetSizer(mainSizer);

    addLayerBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { EndModal(wxID_ADD); });
}
std::string AddElementDialog::GetSelectedLayer()
{
    return layerCombo->GetStringSelection().ToStdString();
}
