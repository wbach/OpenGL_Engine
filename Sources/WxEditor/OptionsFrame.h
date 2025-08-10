#pragma once
#include <GameEngine/Engine/ConfigurationExplorer.h>
#include <wx/notebook.h>
#include <wx/wx.h>

class OptionsFrame : public wxFrame
{
public:
    OptionsFrame(wxWindow*);

private:
    void CreateRenderingOptionsTab(wxNotebook*);
    void CreateRenderingSubTab(wxNotebook*, const std::string&);
    void OnChoiceChanged(wxCommandEvent&);

    void CreateGeneralTab(wxNotebook*);
    void CreateAppearanceTab(wxNotebook*);
    void CreateAdvancedTab(wxNotebook*);

    void OnClose(wxCloseEvent&);

private:
    GameEngine::ConfigurationExplorer configurationExplorer_;

    wxDECLARE_EVENT_TABLE();
};
