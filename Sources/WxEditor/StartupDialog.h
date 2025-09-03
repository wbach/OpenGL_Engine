#pragma once
#include <GameEngine/Scene/SceneFactoryBase.h>
#include <GameEngine/Scene/SceneUtils.h>
#include <Utils/Json/JsonReader.h>
#include <wx/listctrl.h>
#include <wx/wx.h>

#include "Logger/Log.h"
#include "ProjectManager.h"

class StartupDialog : public wxDialog
{
public:
    StartupDialog()
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
        m_recentList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 250), wxLC_REPORT | wxLC_SINGLE_SEL);
        m_recentList->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 200);
        m_recentList->InsertColumn(1, "Path", wxLIST_FORMAT_LEFT, 450);
        PopulateRecentProjects();

        contentSizer->Add(m_recentList, 1, wxALL | wxEXPAND, 10);

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
        m_recentList->Bind(wxEVT_LIST_ITEM_ACTIVATED, &StartupDialog::OnRecentActivated, this);

        // Podpięcie zdarzenia
        removeBtn->Bind(wxEVT_BUTTON, &StartupDialog::OnRemoveSelected, this);

        Bind(wxEVT_CLOSE_WINDOW,
             [this](wxCloseEvent&)
             {
                 EndModal(wxID_CANCEL);  // zwraca CANCEL → App::OnInit() zakończy program
             });
    }

    std::string GetSelectedProject() const
    {
        return m_selectedProject;
    }

private:
    wxListCtrl* m_recentList = nullptr;
    std::string m_selectedProject;

    void PopulateRecentProjects()
    {
        auto recents = ProjectManager::GetInstance().GetRecentProjects();
        long index   = 0;
        for (const auto& proj : recents)
        {
            m_recentList->InsertItem(index, proj.name);
            m_recentList->SetItem(index, 1, proj.path);
            ++index;
        }
    }

    // --- EVENTS ---
    void OnNewProject(wxCommandEvent&)
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
        if (!wxDirExists(projectPath))
            wxMkdir(projectPath);

        auto& projectManager = ProjectManager::GetInstance();
        projectManager.SetProjectPath(projectPath);
        projectManager.SetProjectName(name);
        projectManager.SaveRecentProject(projectPath);

        auto defualtMainScene = projectManager.GetScenesDir() + "/main.xml";
        // SaveSceneAs(defualtMainScene);
        GameEngine::CreateDefaultFile(projectManager.GetConfigFile());
        GameEngine::createScenesFile(projectManager.GetScenesFactoryFile());

        m_selectedProject = projectPath;
        EndModal(wxID_OK);
    }

    void OnOpenProject(wxCommandEvent&)
    {
        auto& pm = ProjectManager::GetInstance();
        wxDirDialog dirDlg(this, "Choose project folder:", pm.GetProjectPath(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

        if (dirDlg.ShowModal() != wxID_OK)
            return;

        std::string path = dirDlg.GetPath().ToStdString();
        pm.SetProjectPath(path);
        m_selectedProject = path;
        pm.SaveRecentProject(path);

        ReadSceneFactoryFile();

        EndModal(wxID_OK);
    }

    void ReadSceneFactoryFile()
    {
        LOG_DEBUG << "ReadSceneFactoryFile";
        auto& pm = ProjectManager::GetInstance();
        Utils::JsonReader jsonReader;

         //wxMessageBox(pm.GetScenesFactoryFile());
        if (jsonReader.Read(pm.GetScenesFactoryFile()))
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
            wxMessageBox(pm.GetScenesFactoryFile());
        }
    }

    void OnExit(wxCommandEvent&)
    {
        EndModal(wxID_CANCEL);
    }

    void OnRecentActivated(wxListEvent& event)
    {
        wxString path     = m_recentList->GetItemText(event.GetIndex(), 1);
        m_selectedProject = path.ToStdString();
        ProjectManager::GetInstance().SetProjectPath(m_selectedProject);
        ReadSceneFactoryFile();
        EndModal(wxID_OK);
    }

    void OnRemoveSelected(wxCommandEvent&)
    {
        long item = m_recentList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item == -1)
        {
            wxMessageBox("No project selected to remove.", "Info", wxOK | wxICON_INFORMATION);
            return;
        }

        // Pobierz ścieżkę projektu
        std::string path = m_recentList->GetItemText(item, 1).ToStdString();

        // Usuń z ProjectManager / wxConfig
        ProjectManager::GetInstance().RemoveRecentProject(path);

        // Usuń z listy
        m_recentList->DeleteItem(item);
    }
};
