#include "OptionsFrame.h"

#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include <GameEngine/Resources/File.h>
#include <GameEngine/Scene/SceneUtils.h>
#include <wx/filepicker.h>
#include <wx/spinctrl.h>

#include <functional>

#include "Logger/Log.h"
#include "Theme.h"
#include "WxEditor/ProjectManager.h"
#include "WxEditor/WxHelpers/EditorUitls.h"

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
    CreateProjectTab(notebook);
    CreateScenesTab(notebook);
    // CreateGeneralTab(notebook);
    CreateAppearanceTab(notebook);
    //    CreateAdvancedTab(notebook);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(notebook, 1, wxEXPAND);
    SetSizer(mainSizer);

    // ? Wymus przeliczenie layoutu po zbudowaniu calej struktury
    Layout();

    Bind(wxEVT_SHOW,
         [this](wxShowEvent& event)
         {
             if (event.IsShown())
             {
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

    // Opcjonalnie wymus odswiezenie rozmiaru wewnetrznego notebooka, jesli potrzebne:
    renderingNotebook->Layout();
    renderingNotebook->SendSizeEvent();

    // Poinformuj renderingPanel o rozmiarze zawartosci:
    renderingPanel->FitInside();

    notebook->AddPage(renderingPanel, "Rendering Options");
}

void OptionsFrame::CreateRenderingSubTab(wxNotebook* notebook, const std::string& catergory)
{
    wxPanel* panel    = new wxPanel(notebook);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    auto& params = configurationExplorer_.getParamsFromCategory(catergory);

    for (auto& param : params)
    {
        wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
        rowSizer->Add(new wxStaticText(panel, wxID_ANY, param.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        wxArrayString options;
        for (const auto& value : param.configurationParam.get().getValuesAsStrings())
        {
            options.Add(value);
        }
        auto choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, options);
        choice->SetSelection(param.configurationParam.get().getValueIndex());
        rowSizer->Add(choice, 0, wxALL, 5);

        paramsCtrls.push_back(ParamCtrl{.param = param.configurationParam.get(), .ctrl = choice});

        if (param.configurationParam.get().isLocked())
        {
            choice->Disable();
        }

        choice->Bind(wxEVT_CHOICE,
                     [this, param](const auto& event)
                     {
                         wxChoice* choice = dynamic_cast<wxChoice*>(event.GetEventObject());
                         if (!choice)
                             return;

                         int sel = choice->GetSelection();
                         param.configurationParam.get().setValueFromIndex(sel);

                         if (param.restartRequierd == GameEngine::ConfigurationExplorer::ApplyPolicy::RestartRequired)
                         {
                             wxMessageBox("To make effect restart is requierd", "Information", wxOK | wxICON_INFORMATION, this);
                         }

                         if (param.paramsImpact == GameEngine::ConfigurationExplorer::ParamsImpact::HasImpact)
                         {
                             UpdateSelectedValuesInCtrl();
                         }

                         WriteConfigurationToFile(EngineConf);
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
    choice->Bind(wxEVT_CHOICE,
                 [&](const auto& event)
                 {
                     wxChoice* choice = dynamic_cast<wxChoice*>(event.GetEventObject());
                     if (!choice)
                         return;
                     int sel = choice->GetSelection();
                     switch (sel)
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
    Hide();        // ukryj zamiast niszczyc
    event.Veto();  // anuluj zamkniecie
}

void OptionsFrame::UpdateSelectedValuesInCtrl()
{
    for (auto& parmCtrl : paramsCtrls)
    {
        auto index = parmCtrl.param.getValueIndex();
        parmCtrl.ctrl->Select(index);
    }
}
void OptionsFrame::CreateProjectTab(wxNotebook* notebook)
{
    wxPanel* panel        = new wxPanel(notebook);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxLogNull logNo;

    auto saveLocal  = []() { ProjectManager::GetInstance().SaveLocalConfigFile(); };
    auto saveEditor = []() { ProjectManager::GetInstance().SaveEditorConfig(); };

    struct ProjectOption
    {
        std::string label;
        std::filesystem::path path;
        std::function<void(const std::string&)> setter;
        std::function<void()> onSave;
        std::string filter = "";  // Jeśli pusty -> wybieramy katalog
        bool isTexture     = false;
    };

    auto AddOptionRow = [=](wxSizer* targetSizer, ProjectOption& opt)
    {
        wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);

        rowSizer->Add(new wxStaticText(panel, wxID_ANY, opt.label), 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

        wxStaticBitmap* thumbnail = nullptr;
        if (opt.isTexture)
        {
            wxImage img;
            auto fullPath = EngineLocalConf.files.getProjectPath() / opt.path;
            if (!opt.path.empty() && img.LoadFile(fullPath.string(), wxBITMAP_TYPE_ANY))
                thumbnail = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(img.Scale(32, 32)));
            else
                thumbnail = new wxStaticBitmap(panel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(32, 32));
            rowSizer->Add(thumbnail, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
        }

        wxTextCtrl* pathCtrl =
            new wxTextCtrl(panel, wxID_ANY, opt.path.string(), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
        rowSizer->Add(pathCtrl, 2, wxEXPAND | wxALL, 5);

        wxButton* browseBtn = new wxButton(panel, wxID_ANY, "...", wxDefaultPosition, wxSize(30, -1));
        rowSizer->Add(browseBtn, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

        targetSizer->Add(rowSizer, 0, wxEXPAND);

        browseBtn->Bind(
            wxEVT_BUTTON,
            [=](wxCommandEvent&)
            {
                std::string resultPath = "";
                bool success           = false;

                if (opt.filter.empty())
                {
                    wxDirDialog dlg(panel, "Select Folder", opt.path.string(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
                    if (dlg.ShowModal() == wxID_OK)
                    {
                        resultPath = dlg.GetPath().ToStdString();
                        success    = true;
                    }
                }
                else
                {
                    wxString startDir = EngineLocalConf.files.getDataPath().string();
                    wxFileDialog dlg(panel, "Select " + opt.label, startDir, "", opt.filter, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                    if (dlg.ShowModal() == wxID_OK)
                    {
                        resultPath = dlg.GetPath().ToStdString();
                        success    = true;
                    }
                }

                if (success)
                {
                    opt.setter(resultPath);
                    pathCtrl->SetValue(resultPath);
                    if (opt.isTexture && thumbnail)
                    {
                        wxImage newImg;
                        if (newImg.LoadFile(resultPath, wxBITMAP_TYPE_ANY))
                        {
                            thumbnail->SetBitmap(wxBitmap(newImg.Scale(32, 32)));
                            thumbnail->Refresh();
                        }
                    }
                    if (opt.onSave)
                        opt.onSave();
                }
            });
    };

    wxStaticBoxSizer* folderSizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, "Project Paths"), wxVERTICAL);
    std::vector<ProjectOption> folderOptions = {
        {"Data Path:", EngineLocalConf.files.getDataPath(), [&](auto p) { EngineLocalConf.files.setDataPath(p); }, saveLocal},
        {"Shader Path:", EngineLocalConf.files.getShaderPath(), [&](auto p) { EngineLocalConf.files.setShaderPath(p); },
         saveLocal},
        {"Includes Path:", ProjectManager::GetInstance().GetEngineIncludesDir(),
         [&](auto p) { ProjectManager::GetInstance().SetEngineIncludesDir(p); }, saveEditor}};
    for (auto& opt : folderOptions)
        AddOptionRow(folderSizer, opt);
    mainSizer->Add(folderSizer, 0, wxEXPAND | wxALL, 10);

    wxStaticBoxSizer* assetSizer = new wxStaticBoxSizer(new wxStaticBox(panel, wxID_ANY, "Assets & Config Files"), wxVERTICAL);
    std::vector<ProjectOption> assetOptions = {
        {"Loading BG:", EngineLocalConf.files.getLoadingBackgroundPath(),
         [&](auto p) { EngineLocalConf.files.setLoadingBackgroundPath(p); }, saveLocal, "Images|*.png;*.jpg", true},
        {"Loading Circle:", EngineLocalConf.files.getLoadingCirclePath(),
         [&](auto p) { EngineLocalConf.files.setLoadingCirclePath(p); }, saveLocal, "Images|*.png;*.jpg", true},
        {"Audio File:", EngineLocalConf.files.getLoadingBackgroundAudioPath(),
         [&](auto p) { EngineLocalConf.files.setLoadingBackgroundAudioPath(p); }, saveLocal, "Audio|*.mp3;*.wav;*.ogg"},
        {"Quests File:", EngineLocalConf.files.getQuestsFilePath(), [&](auto p) { EngineLocalConf.files.setQuestsFile(p); },
         saveLocal, "JSON|*.json"},
        {"Dialog GUI:", EngineLocalConf.files.getDialogGuiPath(), [&](auto p) { EngineLocalConf.files.setDialogGuiPath(p); },
         saveLocal, "GUI|*.gui;*.json"}};
    for (auto& opt : assetOptions)
        AddOptionRow(assetSizer, opt);
    mainSizer->Add(assetSizer, 0, wxEXPAND | wxALL, 10);

    panel->SetSizer(mainSizer);
    panel->Layout();
    notebook->AddPage(panel, "Project Options");
}

void OptionsFrame::CreateScenesTab(wxNotebook* notebook)
{
    wxPanel* panel        = new wxPanel(notebook);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ==== Startup scene ====
    mainSizer->Add(new wxStaticText(panel, wxID_ANY, "Startup Scene:"), 0, wxALL, 5);

    startupChoice_ = new wxChoice(panel, wxID_ANY);
    mainSizer->Add(startupChoice_, 0, wxEXPAND | wxALL, 5);

    startupChoice_->Bind(wxEVT_CHOICE, [&](wxCommandEvent& event)
                         { ProjectManager::GetInstance().SetStartupScene(startupChoice_->GetStringSelection().ToStdString()); });

    // ==== Sceny i ich pliki ====  (ScrolledWindow + StaticBox)
    wxScrolledWindow* scrollWin = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    scrollWin->SetScrollRate(5, 5);

    wxStaticBox* scenesBox = new wxStaticBox(scrollWin, wxID_ANY, "Scenes");
    scenesSizer_           = new wxStaticBoxSizer(scenesBox, wxVERTICAL);

    scrollWin->SetSizer(scenesSizer_);

    // przycisk dodawania
    wxButton* addButton = new wxButton(panel, wxID_ANY, "Add Scene");
    addButton->Bind(wxEVT_BUTTON,
                    [=](wxCommandEvent& evt)
                    {
                        auto dlg = createEntryDialogWithSelectedText(panel, "Scene name:", "Add Scene", "NewScene");

                        if (dlg->ShowModal() == wxID_OK)
                        {
                            auto name = dlg->GetValue().ToStdString();
                            if (!name.empty())
                            {
                                ProjectManager::GetInstance().AddScene(name, "");
                                RebuildScenesList(scrollWin);
                            }
                        }
                    });

    mainSizer->Add(scrollWin, 1, wxEXPAND | wxALL, 10);
    mainSizer->Add(addButton, 0, wxALIGN_RIGHT | wxALL, 5);

    panel->SetSizer(mainSizer);
    panel->Layout();

    // inicjalizacja listy
    RebuildScenesList(scrollWin);

    notebook->AddPage(panel, "Game scenes");
}

void OptionsFrame::RebuildScenesList(wxWindow* parent)
{
    scenesSizer_->Clear(true);
    startupChoice_->Clear();

    for (const auto& [sceneName, scenePath] : ProjectManager::GetInstance().GetScenes())
    {
        startupChoice_->Append(sceneName);
        if (sceneName == ProjectManager::GetInstance().GetStartupScene())
            startupChoice_->SetStringSelection(sceneName);

        wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);

        // Nazwa sceny
        rowSizer->Add(new wxStaticText(parent, wxID_ANY, sceneName), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

        // TextCtrl pokazujacy sciezke
        wxTextCtrl* pathCtrl =
            new wxTextCtrl(parent, wxID_ANY, scenePath.string(), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
        rowSizer->Add(pathCtrl, 1, wxEXPAND | wxALL, 5);

        // Przyciski "Browse" i "Remove"
        wxButton* browseBtn = new wxButton(parent, wxID_ANY, "Browse");
        rowSizer->Add(browseBtn, 0, wxALL, 5);

        wxButton* removeBtn = new wxButton(parent, wxID_ANY, "Remove");
        rowSizer->Add(removeBtn, 0, wxALL, 5);

        // Obsluga Browse
        browseBtn->Bind(wxEVT_BUTTON,
                        [=, scene_name = sceneName](wxCommandEvent&)
                        {
                            wxFileDialog dlg(parent, "Select scene file", ProjectManager::GetInstance().GetProjectPath().string(),
                                             "", "*.xml", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                            if (dlg.ShowModal() == wxID_OK)
                            {
                                std::string newPath = dlg.GetPath().ToStdString();
                                ProjectManager::GetInstance().SetSenePath(scene_name, newPath);
                                pathCtrl->SetValue(dlg.GetPath());
                                WriteConfigurationToFile(EngineConf);
                            }
                        });

        // Obsluga Remove
        removeBtn->Bind(wxEVT_BUTTON,
                        [=, scene_name = sceneName](wxCommandEvent&)
                        {
                            ProjectManager::GetInstance().RemoveScene(scene_name);
                            RebuildScenesList(parent);
                        });

        scenesSizer_->Add(rowSizer, 0, wxEXPAND | wxALL, 2);
    }

    parent->Layout();
    parent->FitInside();
    parent->Refresh();
}
