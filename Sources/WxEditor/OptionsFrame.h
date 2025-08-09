#pragma once
#include <wx/wx.h>
#include <wx/notebook.h>

class OptionsFrame : public wxFrame
{
public:
    OptionsFrame(wxWindow* parent);

private:
    void CreateGeneralTab(wxNotebook* notebook);
    void CreateAppearanceTab(wxNotebook* notebook);
    void CreateAdvancedTab(wxNotebook* notebook);

    void OnClose(wxCloseEvent& event);

    wxDECLARE_EVENT_TABLE();
};
