#pragma once
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/txtstrm.h>

#include <functional>
#include <filesystem>

class ProjectPanel;

class BuildComponentLogFrame : public wxFrame
{
public:
    using ReloadComponents = std::function<void()>;

    BuildComponentLogFrame(wxWindow*, ProjectPanel*, ReloadComponents);
    void Build();
    void AppendLine(const wxString& line, const wxColour& colour = *wxBLACK);

private:
    void RunCommand(const std::string&, const std::filesystem::path&, wxProcess*);

private:
    wxTextCtrl* textCtrl;
    ProjectPanel* projectPanel;
    ReloadComponents reloadComponents;
};
