#pragma once

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/wx.h>

class TerrainToolPanel : public wxScrolledWindow
{
public:
    TerrainToolPanel(wxWindow* parent, int width);
    ~TerrainToolPanel() override = default;

    void ShowPanel(bool show);
    bool IsVisible() const
    {
        return isVisible;
    }

private:
    void OnClose(wxCommandEvent&);
    void BuildUI();
    void BuildTerrainGeneratorUI(wxSizer*);
    void BuildTerrainPainterUI(wxSizer*);

private:
    bool isVisible = false;

    wxTextCtrl* gameObjectIdCtrl;
    wxTextCtrl* biasCtrl;
    wxTextCtrl* octavesCtrl;
    wxTextCtrl* widthCtrl;

    wxDECLARE_EVENT_TABLE();
};
