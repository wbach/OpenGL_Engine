#pragma once
#include <wx/listctrl.h>
#include <wx/wx.h>

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
        wxButton* exitBtn       = new wxButton(this, wxID_EXIT, "Exit");

        buttonSizer->Add(newBtn, 0, wxEXPAND | wxALL, 5);
        buttonSizer->Add(openBtn, 0, wxEXPAND | wxALL, 5);
        buttonSizer->Add(exitBtn, 0, wxEXPAND | wxALL, 5);

        contentSizer->Add(buttonSizer, 0, wxALIGN_TOP | wxALL, 10);
        mainSizer->Add(contentSizer, 1, wxEXPAND);

        // FOOTER
        wxBoxSizer* footerSizer = new wxBoxSizer(wxHORIZONTAL);
        wxButton* loadBtn       = new wxButton(this, wxID_OK, "Load Selected Project");
        footerSizer->AddStretchSpacer(1);
        footerSizer->Add(loadBtn, 0, wxALL, 10);
        mainSizer->Add(footerSizer, 0, wxEXPAND);

        SetSizerAndFit(mainSizer);
        Centre();

        // BIND EVENTS
        newBtn->Bind(wxEVT_BUTTON, &StartupDialog::OnNewProject, this);
        openBtn->Bind(wxEVT_BUTTON, &StartupDialog::OnOpenProject, this);
        exitBtn->Bind(wxEVT_BUTTON, &StartupDialog::OnExit, this);
        loadBtn->Bind(wxEVT_BUTTON, &StartupDialog::OnLoadSelected, this);
        m_recentList->Bind(wxEVT_LIST_ITEM_ACTIVATED, &StartupDialog::OnRecentActivated, this);

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

        ProjectManager::GetInstance().SetProjectPath(projectPath);
        ProjectManager::GetInstance().SetProjectName(name);

        ProjectManager::GetInstance().SaveRecentProject(projectPath);

        m_selectedProject = projectPath;
        EndModal(wxID_OK);
    }

    void OnOpenProject(wxCommandEvent&)
    {
        wxDirDialog dirDlg(this, "Choose project folder:", ProjectManager::GetInstance().GetProjectPath(),
                           wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

        if (dirDlg.ShowModal() != wxID_OK)
            return;

        std::string path = dirDlg.GetPath().ToStdString();
        ProjectManager::GetInstance().SetProjectPath(path);
        m_selectedProject = path;

        ProjectManager::GetInstance().SaveRecentProject(path);

        EndModal(wxID_OK);
    }

    void OnExit(wxCommandEvent&)
    {
        EndModal(wxID_CANCEL);
    }

    void OnLoadSelected(wxCommandEvent&)
    {
        long sel = m_recentList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (sel == -1)
        {
            wxMessageBox("Select a project first.");
            return;
        }

        wxString path     = m_recentList->GetItemText(sel, 1);
        m_selectedProject = path.ToStdString();
        ProjectManager::GetInstance().SetProjectPath(m_selectedProject);

        EndModal(wxID_OK);
    }

    void OnRecentActivated(wxListEvent& event)
    {
        wxString path     = m_recentList->GetItemText(event.GetIndex(), 1);
        m_selectedProject = path.ToStdString();
        ProjectManager::GetInstance().SetProjectPath(m_selectedProject);

        EndModal(wxID_OK);
    }
};
