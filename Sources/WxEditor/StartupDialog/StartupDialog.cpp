#include "StartupDialog.h"

#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ConfigurationWriter.h>
#include <GameEngine/Scene/SceneFactoryBase.h>
#include <GameEngine/Scene/SceneUtils.h>
#include <Logger/Log.h>
#include <Utils/Json/JsonReader.h>
#include <wx/defs.h>
#include <wx/log.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>
#include <fstream>

#include "ProjectsCMakeTemplate.h"
#include "WxEditor/ProjectManager.h"

StartupDialog::StartupDialog()
    : wxDialog(nullptr, wxID_ANY, "MyEngine Launcher", wxDefaultPosition, wxSize(700, 450))
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // HEADER
    wxPanel* header = new wxPanel(this);
    header->SetBackgroundColour(wxColour(45, 45, 48));
    wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* title     = new wxStaticText(header, wxID_ANY, "MyEngine Editor");
    title->SetFont(wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    title->SetForegroundColour(*wxWHITE);
    headerSizer->Add(title, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);
    header->SetSizer(headerSizer);
    mainSizer->Add(header, 0, wxEXPAND);

    // CONTENT
    wxBoxSizer* contentSizer = new wxBoxSizer(wxHORIZONTAL);

    // Recent projects list
    recentList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 250), wxLC_REPORT | wxLC_SINGLE_SEL);
    recentList->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 200);
    recentList->InsertColumn(1, "Path", wxLIST_FORMAT_LEFT, 450);
    PopulateRecentProjects();

    contentSizer->Add(recentList, 1, wxALL | wxEXPAND, 10);

    // Buttons panel
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
    wxButton* newBtn        = new wxButton(this, wxID_NEW, "New Project");
    wxButton* openBtn       = new wxButton(this, wxID_OPEN, "Open Project");
    wxButton* removeBtn     = new wxButton(this, wxID_ANY, "Remove selected from list");

    buttonSizer->Add(newBtn, 0, wxEXPAND | wxALL, 5);
    buttonSizer->Add(openBtn, 0, wxEXPAND | wxALL, 5);
    buttonSizer->Add(removeBtn, 0, wxEXPAND | wxALL, 5);

    contentSizer->Add(buttonSizer, 0, wxALIGN_TOP | wxALL, 10);
    mainSizer->Add(contentSizer, 1, wxEXPAND);

    // FOOTER
    wxBoxSizer* footerSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* exitBtn       = new wxButton(this, wxID_EXIT, "Exit");
    footerSizer->AddStretchSpacer(1);
    footerSizer->Add(exitBtn, 0, wxALL, 10);
    mainSizer->Add(footerSizer, 0, wxEXPAND);

    SetSizerAndFit(mainSizer);
    Centre();

    // BIND EVENTS
    newBtn->Bind(wxEVT_BUTTON, &StartupDialog::OnNewProject, this);
    openBtn->Bind(wxEVT_BUTTON, &StartupDialog::OnOpenProject, this);
    exitBtn->Bind(wxEVT_BUTTON, &StartupDialog::OnExit, this);
    recentList->Bind(wxEVT_LIST_ITEM_ACTIVATED, &StartupDialog::OnRecentActivated, this);

    // Podpiecie zdarzenia
    removeBtn->Bind(wxEVT_BUTTON, &StartupDialog::OnRemoveSelected, this);

    Bind(wxEVT_CLOSE_WINDOW,
         [this](wxCloseEvent&)
         {
             EndModal(wxID_CANCEL);  // zwraca CANCEL -> App::OnInit() zakonczy program
         });
}
std::string StartupDialog::GetSelectedProject() const
{
    return selectedProject;
}
void StartupDialog::PopulateRecentProjects()
{
    auto recents = ProjectManager::GetInstance().GetRecentProjects();
    long index   = 0;
    for (const auto& proj : recents)
    {
        recentList->InsertItem(index, proj.name);
        recentList->SetItem(index, 1, proj.path);
        ++index;
    }
}
void StartupDialog::OnNewProject(wxCommandEvent&)
{
    wxTextEntryDialog nameDlg(this, "Enter project name:");
    if (nameDlg.ShowModal() != wxID_OK)
        return;

    std::string name = nameDlg.GetValue().ToStdString();
    if (name.empty())
    {
        wxMessageBox("Project name cannot be empty!");
        return;
    }

    wxDirDialog dirDlg(this, "Choose base folder:");
    if (dirDlg.ShowModal() != wxID_OK)
        return;

    std::string folder      = dirDlg.GetPath().ToStdString();
    std::string projectPath = folder + "/" + name;
    if (not std::filesystem::exists(projectPath))
    {
        Utils::CreateDirectories(projectPath);
    }

    auto& projectManager = ProjectManager::GetInstance();
    projectManager.SetProjectPath(projectPath);
    projectManager.SetProjectName(name);
    projectManager.SaveRecentProject(projectPath);

    GameEngine::WriteConfigurationToFile(EngineConf, projectManager.GetConfigFile().string());
    GameEngine::createScenesFile(projectManager.GetScenesFactoryFile());

    std::ofstream out(projectManager.GetProjectPath() / "CMakeLists.txt");
    if (out)
    {
        out << projectCMakeTemplate;
    }

    selectedProject = projectPath;
    EndModal(wxID_OK);
}
void StartupDialog::OnOpenProject(wxCommandEvent&)
{
    auto& pm = ProjectManager::GetInstance();
    wxDirDialog dirDlg(this, "Choose project folder:", pm.GetProjectPath().string(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dirDlg.ShowModal() != wxID_OK)
        return;

    std::string path = dirDlg.GetPath().ToStdString();
    pm.SetProjectPath(path);
    selectedProject = path;
    pm.SaveRecentProject(path);

    ReadSceneFactoryFile();

    EndModal(wxID_OK);
}
void StartupDialog::ReadSceneFactoryFile()
{
    LOG_DEBUG << "ReadSceneFactoryFile";
    auto& pm = ProjectManager::GetInstance();
    Utils::JsonReader jsonReader;

    if (jsonReader.Read(pm.GetScenesFactoryFile().string()))
    {
        const std::string CSTR_ROOT_NODE{"projectConfiguration"};
        const std::string CSTR_STARTUP_SCENE_NODE{"startupScene"};
        const std::string CSTR_SCENES_NODE{"scenes"};

        auto startupScene = jsonReader.Get(CSTR_STARTUP_SCENE_NODE);
        if (startupScene)
        {
            pm.SetStartupScene(startupScene->value_);
        }
        else
        {
            LOG_DEBUG << "Startup node not found";
        }

        auto scenesNode = jsonReader.Get(CSTR_SCENES_NODE);
        if (scenesNode)
        {
            LOG_DEBUG << "scenes size=" << scenesNode->getChildren().size();
            for (const auto& sceneNode : scenesNode->getChildren())
            {
                if (not sceneNode)
                    continue;

                pm.AddScene(sceneNode->name(), sceneNode->value_);
            }
        }
        else
        {
            LOG_DEBUG << "scenes node not found";
        }
    }
    else
    {
        LOG_DEBUG << "ReadSceneFactoryFile failed: " << pm.GetScenesFactoryFile();
        wxMessageBox(pm.GetScenesFactoryFile().string());
    }
}
void StartupDialog::OnExit(wxCommandEvent&)
{
    EndModal(wxID_CANCEL);
}
void StartupDialog::OnRecentActivated(wxListEvent& event)
{
    wxString path = recentList->GetItemText(event.GetIndex(), 1);
    if (not std::filesystem::exists(path.ToStdString()))
    {
        int answer = wxMessageBox("Project not exist anymore. Path= " + path + ". Do you want remove from recenet projects?",
                                  "Confirmation", wxYES_NO | wxICON_ERROR);

        if (answer == wxYES)
        {
            long item = recentList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            ProjectManager::GetInstance().RemoveRecentProject(path.ToStdString());
            recentList->DeleteItem(item);
        }
        return;
    }
    selectedProject = path.ToStdString();
    ProjectManager::GetInstance().SetProjectPath(selectedProject);
    ProjectManager::GetInstance().SetProjectName(recentList->GetItemText(event.GetIndex(), 0).ToStdString());
    ReadSceneFactoryFile();
    EndModal(wxID_OK);
}
void StartupDialog::OnRemoveSelected(wxCommandEvent&)
{
    long item = recentList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item == -1)
    {
        wxMessageBox("No project selected to remove.", "Info", wxOK | wxICON_INFORMATION);
        return;
    }

    // Pobierz sciezke projektu
    std::string path = recentList->GetItemText(item, 1).ToStdString();

    // Usun z ProjectManager / wxConfig
    ProjectManager::GetInstance().RemoveRecentProject(path);

    // Usun z listy
    recentList->DeleteItem(item);
}
