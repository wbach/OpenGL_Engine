#pragma once

#include <wx/listctrl.h>
#include <wx/wx.h>

class StartupDialog : public wxDialog
{
public:
    StartupDialog();

    std::string GetSelectedProject() const;

private:
    void PopulateRecentProjects();
    // --- EVENTS ---
    void OnNewProject(wxCommandEvent&);
    void OnOpenProject(wxCommandEvent&);
    void ReadSceneFactoryFile();
    void OnExit(wxCommandEvent&);
    void OnRecentActivated(wxListEvent& event);
    void OnRemoveSelected(wxCommandEvent&);

private:
    wxListCtrl* recentList = nullptr;
    std::string selectedProject;
};
