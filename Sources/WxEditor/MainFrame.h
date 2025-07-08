
#pragma once
#include <wx/frame.h>
#include <wx/generic/dirctrlg.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

class GLCanvas;

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnAddObject(wxCommandEvent&);
    void OnGLVersion(wxCommandEvent&);
    void OnExit(wxCommandEvent&);
    void OnFileSelectChanged(wxTreeEvent&);
    void OnFileActivated(wxTreeEvent&);

private:
    GLCanvas* canvas;
    wxTreeCtrl* gameObjectsView;
    wxTreeItemId treeRootId;
    wxArrayTreeItemIds gameObjectsItemsIds;
    wxGenericDirCtrl* fileExplorer;
    wxDECLARE_EVENT_TABLE();
};
