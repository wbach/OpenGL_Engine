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
    void CreateProjectTab(wxNotebook*);
    void CreateTextureOptions(wxBoxSizer*);

    void CreateGeneralTab(wxNotebook*);
    void CreateAppearanceTab(wxNotebook*);
    void CreateAdvancedTab(wxNotebook*);

    void OnClose(wxCloseEvent&);

    void UpdateSelectedValuesInCtrl();

private:
    GameEngine::ConfigurationExplorer configurationExplorer_;

    struct ParamCtrl
    {
        GameEngine::Params::IConfigurationParam& param;
        wxChoice* ctrl;
    };
    std::vector<ParamCtrl> paramsCtrls;

    wxDECLARE_EVENT_TABLE();
};
