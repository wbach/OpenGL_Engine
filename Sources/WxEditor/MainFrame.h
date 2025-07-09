
#pragma once
#include <wx/frame.h>
#include <wx/generic/dirctrlg.h>
#include <wx/treectrl.h>
#include <wx/wx.h>
#include <wx/treebase.h>
#include <unordered_map>
#include <Types.h>

class GLCanvas;
namespace GameEngine
{
class GameObject;
}

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnAddObject(wxCommandEvent&);
    void OnGLVersion(wxCommandEvent&);
    void OnOpenScene(wxCommandEvent&);
    void OnExit(wxCommandEvent&);
    void OnFileSelectChanged(wxTreeEvent&);
    void OnFileActivated(wxTreeEvent&);

    void AddChilds(GameEngine::GameObject&, wxTreeItemId);
    void CreateRootGameObject();

private:
    GLCanvas* canvas;
    wxTreeCtrl* gameObjectsView;
    wxTreeItemId treeRootId;
    wxArrayTreeItemIds gameObjectsItemsIds;
    std::unordered_map<IdType, wxTreeItemId> gameObjectsItemsIdsMap;
    wxGenericDirCtrl* fileExplorer;
    wxDECLARE_EVENT_TABLE();
};
