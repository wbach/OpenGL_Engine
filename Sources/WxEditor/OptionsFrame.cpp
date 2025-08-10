#include "OptionsFrame.h"

#include <wx/spinctrl.h>
#include <GameEngine/Engine/Configuration.h>

// clang-format off
wxBEGIN_EVENT_TABLE(OptionsFrame, wxFrame)
    EVT_CLOSE(OptionsFrame::OnClose)
wxEND_EVENT_TABLE()

OptionsFrame::OptionsFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, "Preferences", wxDefaultPosition, wxSize(600, 400))
// clang-format on
{
    EngineConf.renderer.resolution.lock();
    EngineConf.window.fullScreen.lock();
    EngineConf.window.size.lock();

    wxNotebook* notebook = new wxNotebook(this, wxID_ANY);

    CreateRenderingOptionsTab(notebook);
//    CreateGeneralTab(notebook);
//    CreateAppearanceTab(notebook);
//    CreateAdvancedTab(notebook);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(notebook, 1, wxEXPAND);
    SetSizer(mainSizer);

    // 🔹 Wymuś przeliczenie layoutu po zbudowaniu całej struktury
    Layout();

    Bind(wxEVT_SHOW, [this](wxShowEvent& event) {
        if (event.IsShown()) {
            SetSize(800, 600);
            SendSizeEvent();
        }
        event.Skip();
    });
}

void OptionsFrame::CreateRenderingOptionsTab(wxNotebook* notebook)
{
    wxScrolledWindow* renderingPanel = new wxScrolledWindow(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    renderingPanel->SetScrollRate(5, 5);

    wxBoxSizer* renderingSizer = new wxBoxSizer(wxVERTICAL);

    wxNotebook* renderingNotebook = new wxNotebook(renderingPanel, wxID_ANY);

    auto categories = configurationExplorer_.getCatogiresList();
    for (auto& category : categories)
    {
        CreateRenderingSubTab(renderingNotebook, category);
    }

    renderingSizer->Add(renderingNotebook, 1, wxEXPAND | wxALL, 5);
    renderingPanel->SetSizer(renderingSizer);

    renderingPanel->Layout();

    // Opcjonalnie wymuś odświeżenie rozmiaru wewnętrznego notebooka, jeśli potrzebne:
    renderingNotebook->Layout();
    renderingNotebook->SendSizeEvent();

    // Poinformuj renderingPanel o rozmiarze zawartości:
    renderingPanel->FitInside();

    notebook->AddPage(renderingPanel, "Rendering Options");
}

void OptionsFrame::CreateRenderingSubTab(wxNotebook * notebook, const std::string & catergory)
{
    wxPanel* panel    = new wxPanel(notebook);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    const auto& params = configurationExplorer_.getParamsFromCategory(catergory);

    for (auto& param : params)
    {
        wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
        rowSizer->Add(new wxStaticText(panel, wxID_ANY, param.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxArrayString themes;
        for(const auto& value : param.configurationParam.getValuesAsStrings())
        {
            themes.Add(value);
        }
        auto choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, themes);
        choice->SetSelection(param.configurationParam.getValueIndex());
        rowSizer->Add(choice, 0, wxALL, 5);

        if (param.configurationParam.isLocked())
        {
            choice->Disable();
        }
        choice->SetClientData(&param.configurationParam);
        choice->Bind(wxEVT_CHOICE, &OptionsFrame::OnChoiceChanged, this);
        sizer->Add(rowSizer, 0, wxEXPAND | wxTOP, 2);
    }

    panel->SetSizer(sizer);
    panel->Layout();

    notebook->AddPage(panel, catergory);
}

void OptionsFrame::OnChoiceChanged(wxCommandEvent &event)
{
    wxChoice* choice = dynamic_cast<wxChoice*>(event.GetEventObject());
    if (!choice) return;

    auto paramPtr = static_cast<GameEngine::Params::IConfigurationParam*>(choice->GetClientData());
    if (paramPtr)
    {
        int sel = choice->GetSelection();
        paramPtr->setValueFromIndex(sel);
    }
}

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

void OptionsFrame::OnClose(wxCloseEvent& event)
{
    Hide();        // ukryj zamiast niszczyć
    event.Veto();  // anuluj zamknięcie
}
