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
        for (const auto& value : param.configurationParam.getValuesAsStrings())
        {
            options.Add(value);
        }
        auto choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, options);
        choice->SetSelection(param.configurationParam.getValueIndex());
        rowSizer->Add(choice, 0, wxALL, 5);

        paramsCtrls.push_back(ParamCtrl{.param = param.configurationParam, .ctrl = choice});

        if (param.configurationParam.isLocked())
        {
            choice->Disable();
        }
        choice->SetClientData(&param.configurationParam);
        choice->Bind(wxEVT_CHOICE,
                     [this, param](const auto& event)
                     {
                         wxChoice* choice = dynamic_cast<wxChoice*>(event.GetEventObject());
                         if (!choice)
                             return;

                         auto paramPtr = static_cast<GameEngine::Params::IConfigurationParam*>(choice->GetClientData());
                         if (paramPtr)
                         {
                             int sel = choice->GetSelection();
                             paramPtr->setValueFromIndex(sel);

                             if (param.restartRequierd == GameEngine::ConfigurationExplorer::ApplyPolicy::RestartRequired)
                             {
                                 wxMessageBox("To make effect restart is requierd", "Information", wxOK | wxICON_INFORMATION,
                                              this);
                             }

                             if (param.paramsImpact == GameEngine::ConfigurationExplorer::ParamsImpact::HasImpact)
                             {
                                 UpdateSelectedValuesInCtrl();
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

    // ===== Sekcja katalogow =====
    wxStaticBox* folderBox        = new wxStaticBox(panel, wxID_ANY, "Project Paths");
    wxStaticBoxSizer* folderSizer = new wxStaticBoxSizer(folderBox, wxVERTICAL);

    struct PathOption
    {
        std::string label;
        std::filesystem::path value;
        std::function<void(const std::filesystem::path&)> setValue;
        std::function<void()> saveConfig;
    };

    auto saveConfig       = []() { ProjectManager::GetInstance().SaveLocalConfigFile(); };
    auto saveEditorConfig = []() { ProjectManager::GetInstance().SaveEditorConfig(); };

    std::vector<PathOption> paths = {
        {"Data path:", EngineLocalConf.files.getDataPath(), [&](const auto& path) { EngineLocalConf.files.setDataPath(path); },
         saveConfig},
        {"Shader path:", EngineLocalConf.files.getShaderPath(),
         [&](const auto& path) { EngineLocalConf.files.setShaderPath(path); }, saveConfig},
        {"Engine includes path:", ProjectManager::GetInstance().GetEngineIncludesDir(),
         [](const auto& path) { ProjectManager::GetInstance().SetEngineIncludesDir(path); }, saveEditorConfig}};

    for (auto& pathOpt : paths)
    {
        folderSizer->Add(new wxStaticText(panel, wxID_ANY, pathOpt.label), 0, wxALL, 5);

        wxDirPickerCtrl* dirPicker = new wxDirPickerCtrl(panel, wxID_ANY, pathOpt.value.string(), "Select a folder");
        folderSizer->Add(dirPicker, 0, wxEXPAND | wxALL, 5);

        wxStaticText* pathDisplay = new wxStaticText(panel, wxID_ANY, pathOpt.value.string());
        folderSizer->Add(pathDisplay, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

        dirPicker->Bind(wxEVT_DIRPICKER_CHANGED,
                        [&, option = pathOpt, dirPicker, pathDisplay](wxFileDirPickerEvent& event)
                        {
                            auto newValue = dirPicker->GetPath().ToStdString();
                            option.setValue(newValue);
                            pathDisplay->SetLabel(newValue);
                            saveConfig();
                        });
    }

    mainSizer->Add(folderSizer, 0, wxEXPAND | wxALL, 10);  // margines miedzy sekcjami

    wxLogNull logNo;
    // ===== Sekcja tekstur =====
    wxStaticBox* textureBox        = new wxStaticBox(panel, wxID_ANY, "Textures");
    wxStaticBoxSizer* textureSizer = new wxStaticBoxSizer(textureBox, wxVERTICAL);

    struct TextureOption
    {
        std::string label;
        std::filesystem::path path;
        std::function<void(const std::filesystem::path&)> setValue;
    };

    std::vector<TextureOption> textures = {{"Loading screen background:", EngineLocalConf.files.getLoadingBackgroundPath(),
                                            [&](const auto& path) { EngineLocalConf.files.setLoadingBackgroundPath(path); }},
                                           {"Loading circle texture:", EngineLocalConf.files.getLoadingCirclePath(),
                                            [&](const auto& path) { EngineLocalConf.files.setLoadingCirclePath(path); }}};

    for (auto& texOpt : textures)
    {
        wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);

        wxImage img;
        if (!texOpt.path.empty())
            img.LoadFile((EngineLocalConf.files.getProjectPath() / texOpt.path).string(), wxBITMAP_TYPE_ANY);
        wxStaticBitmap* thumbnail = new wxStaticBitmap(panel, wxID_ANY, img.IsOk() ? wxBitmap(img.Scale(50, 50)) : wxNullBitmap);

        rowSizer->Add(thumbnail, 0, wxALL, 5);

        wxTextCtrl* pathCtrl =
            new wxTextCtrl(panel, wxID_ANY, texOpt.path.string(), wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
        rowSizer->Add(pathCtrl, 1, wxEXPAND | wxALL, 5);

        wxButton* browseBtn = new wxButton(panel, wxID_ANY, "Browse");
        rowSizer->Add(browseBtn, 0, wxALL, 5);

        textureSizer->Add(rowSizer, 0, wxEXPAND | wxALL, 5);

        browseBtn->Bind(
            wxEVT_BUTTON,
            [=, text = texOpt](wxCommandEvent&) mutable
            {
                wxString startDir = EngineLocalConf.files.getDataPath().string();
                wxFileDialog dlg(panel, "Select texture", startDir, "", "*.png;*.jpg;*.bmp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

                if (dlg.ShowModal() == wxID_OK)
                {
                    text.setValue(dlg.GetPath().ToStdString());
                    text.path = dlg.GetPath().ToStdString();
                    pathCtrl->SetValue(dlg.GetPath());
                    wxImage newImg;
                    if (newImg.LoadFile((EngineLocalConf.files.getProjectPath() / text.path).string(), wxBITMAP_TYPE_ANY))
                    {
                        thumbnail->SetBitmap(wxBitmap(newImg.Scale(50, 50)));
                        thumbnail->Refresh();
                    }
                    ProjectManager::GetInstance().SaveLocalConfigFile();
                }
            });
    }

    mainSizer->Add(textureSizer, 0, wxEXPAND | wxALL, 10);  // margines na dole sekcji

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
