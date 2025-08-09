#include "OptionsFrame.h"
#include <wx/spinctrl.h>

wxBEGIN_EVENT_TABLE(OptionsFrame, wxFrame)
    EVT_CLOSE(OptionsFrame::OnClose)
wxEND_EVENT_TABLE()

OptionsFrame::OptionsFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, "Opcje programu", wxDefaultPosition, wxSize(600, 400))
{
    wxNotebook* notebook = new wxNotebook(this, wxID_ANY);

    CreateGeneralTab(notebook);
    CreateAppearanceTab(notebook);
    CreateAdvancedTab(notebook);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(notebook, 1, wxEXPAND);
    SetSizer(mainSizer);
}

// --- Zakładka "Ogólne"
void OptionsFrame::CreateGeneralTab(wxNotebook* notebook)
{
    wxPanel* panel = new wxPanel(notebook);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxCheckBox(panel, wxID_ANY, "Uruchamiaj z systemem"), 0, wxALL, 5);
    sizer->Add(new wxCheckBox(panel, wxID_ANY, "Automatyczny zapis"), 0, wxALL, 5);

    panel->SetSizer(sizer);
    panel->Layout();
    notebook->AddPage(panel, "General");
}

// --- Zakładka "Wygląd"
void OptionsFrame::CreateAppearanceTab(wxNotebook* notebook)
{
    wxPanel* panel = new wxPanel(notebook);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Motyw kolorystyczny:"), 0, wxALL, 5);

    wxArrayString themes;
    themes.Add("Jasny");
    themes.Add("Ciemny");
    themes.Add("Systemowy");
    sizer->Add(new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, themes), 0, wxALL, 5);

    panel->SetSizer(sizer);
    panel->Layout();
    notebook->AddPage(panel, "Wyglad");
}

// --- Zakładka "Zaawansowane"
void OptionsFrame::CreateAdvancedTab(wxNotebook* notebook)
{
    wxPanel* panel = new wxPanel(notebook);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Limit watkow:"), 0, wxALL, 5);
    sizer->Add(new wxSpinCtrl(panel, wxID_ANY, "4", wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 1, 64), 0, wxALL, 5);

    panel->SetSizer(sizer);
    panel->Layout();
    notebook->AddPage(panel, "Zaawansowane");
}

void OptionsFrame::OnClose(wxCloseEvent &event)
{
    Hide(); // ukryj zamiast niszczyć
    event.Veto(); // anuluj zamknięcie
}
