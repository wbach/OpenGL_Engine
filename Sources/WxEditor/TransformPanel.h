#pragma once
#include <Common/Transform.h>
#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/collpane.h>

class TransformPanel : public wxPanel
{
public:
    struct Vector3Controls
    {
        wxTextCtrl* xCtrl;
        wxTextCtrl* yCtrl;
        wxTextCtrl* zCtrl;
        wxSizer*    sizer; // żeby można było łatwo dodać do layoutu
    };

    TransformPanel(wxWindow* parent);
    void set(const common::Transform&);

private:
    void OnPaneChanged(wxCollapsiblePaneEvent&);
    Vector3Controls CreateVector3Controls(wxWindow*, const wxString&);

private:
    wxCollapsiblePane* posPane;
    wxCollapsiblePane* rotPane;
    wxCollapsiblePane* scalePane;

    Vector3Controls position;
    Vector3Controls rotation;
    Vector3Controls scale;
};
