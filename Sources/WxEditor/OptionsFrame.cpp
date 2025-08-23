#include "OptionsFrame.h"

#include <wx/spinctrl.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include "Theme.h"

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
    //CreateGeneralTab(notebook);
    CreateAppearanceTab(notebook);
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

        wxArrayString options;
        for(const auto& value : param.configurationParam.getValuesAsStrings())
        {
            options.Add(value);
        }
        auto choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, options);
        choice->SetSelection(param.configurationParam.getValueIndex());
        rowSizer->Add(choice, 0, wxALL, 5);

        if (param.configurationParam.isLocked())
        {
            choice->Disable();
        }
        choice->SetClientData(&param.configurationParam);
        choice->Bind(wxEVT_CHOICE, [this, param](const auto& event){
            wxChoice* choice = dynamic_cast<wxChoice*>(event.GetEventObject());
            if (!choice) return;

            auto paramPtr = static_cast<GameEngine::Params::IConfigurationParam*>(choice->GetClientData());
            if (paramPtr)
            {
                int sel = choice->GetSelection();
                paramPtr->setValueFromIndex(sel);

                if (param.restartRequierd == GameEngine::ConfigurationExplorer::ApplyPolicy::RestartRequired)
                {
                    wxMessageBox("To make effect restart is requierd", "Information",
                                              wxOK| wxICON_INFORMATION,  this);
                }
                WriteConfigurationToFile(EngineConf);
            }
        });
        sizer->Add(rowSizer, 0, wxEXPAND | wxTOP, 2);
    }

    panel->SetSizer(sizer);
    panel->Layout();

    notebook->AddPage(panel, catergory);
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
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Theme:"), 0, wxALL, 5);

    wxArrayString themes;
    themes.Add("Light");
    themes.Add("Dark");
    themes.Add("OsBased");
    auto choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, themes);
    sizer->Add(choice, 0, wxALL, 5);
    choice->SetSelection(2);
    choice->Bind(wxEVT_CHOICE, [&](const auto& event){
        wxChoice* choice = dynamic_cast<wxChoice*>(event.GetEventObject());
        if (!choice) return;
        int sel = choice->GetSelection();
        switch(sel)
        {
        case 0:
            ApplyTheme(*this->GetParent(), LIGHT_THEME);
            break;
        case 1:
            ApplyTheme(*this->GetParent(), DARK_THEME);
            break;
        case 2:
            ApplyTheme(*this->GetParent(), OS_DEFAULT_THEME);
            break;
        }

    });

    panel->SetSizer(sizer);
    panel->Layout();
    notebook->AddPage(panel, "Enviroment");
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
