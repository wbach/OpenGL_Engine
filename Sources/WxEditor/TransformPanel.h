#pragma once

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/collpane.h>

class TransformPanel : public wxPanel
{
public:
    TransformPanel(wxWindow* parent);

private:
    void OnPaneChanged(wxCollapsiblePaneEvent& event);

private:
    wxCollapsiblePane* posPane;
    wxCollapsiblePane* rotPane;
    wxCollapsiblePane* scalePane;
};
