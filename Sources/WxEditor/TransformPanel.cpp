#include "TransformPanel.h"
#include <wx/textctrl.h>
#include <wx/sizer.h>

TransformPanel::TransformPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    wxCollapsiblePane* collapsible = new wxCollapsiblePane(this, wxID_ANY, "Transform");

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(collapsible, 1, wxEXPAND | wxALL, 5);

    wxWindow* pane = collapsible->GetPane();

    auto createVector3Controls = [](wxWindow* parent, const wxString& label) -> wxSizer*
    {
        // Statyczny box z labelką (nagłówek)
        wxStaticBox* box = new wxStaticBox(parent, wxID_ANY, label);
        wxStaticBoxSizer* boxSizer = new wxStaticBoxSizer(box, wxVERTICAL);

        // Wiersz z polami X Y Z
        wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
        rowSizer->Add(new wxStaticText(parent, wxID_ANY, "X:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        rowSizer->Add(new wxTextCtrl(parent, wxID_ANY, "0"), 1, wxRIGHT, 10);
        rowSizer->Add(new wxStaticText(parent, wxID_ANY, "Y:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        rowSizer->Add(new wxTextCtrl(parent, wxID_ANY, "0"), 1, wxRIGHT, 10);
        rowSizer->Add(new wxStaticText(parent, wxID_ANY, "Z:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        rowSizer->Add(new wxTextCtrl(parent, wxID_ANY, "0"), 1);

        boxSizer->Add(rowSizer, 0, wxEXPAND | wxALL, 5);

        return boxSizer;
    };

    wxBoxSizer* contentSizer = new wxBoxSizer(wxVERTICAL);
    contentSizer->Add(createVector3Controls(pane, "Position"), 0, wxEXPAND | wxALL, 5);
    contentSizer->Add(createVector3Controls(pane, "Rotation"), 0, wxEXPAND | wxALL, 5);
    contentSizer->Add(createVector3Controls(pane, "Scale"),    0, wxEXPAND | wxALL, 5);

    pane->SetSizer(contentSizer);

    SetSizer(mainSizer);
    Layout();

    collapsible->Collapse(false);
}
