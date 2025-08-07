
#pragma once
#include <Types.h>
#include <wx/frame.h>
#include <wx/generic/dirctrlg.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

#include <thread>
#include <unordered_map>

class GLCanvas;
namespace GameEngine
{
class GameObject;
}

struct TreeItemIdHasher
{
    std::size_t operator()(const wxTreeItemId& id) const
    {
        return std::hash<void*>()(id.GetID());
    }
};

struct TreeItemIdEqual
{
    bool operator()(const wxTreeItemId& lhs, const wxTreeItemId& rhs) const
    {
        return lhs == rhs;
    }
};

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void MenuFileOpenScene(wxCommandEvent&);
    void MenuFileReloadScene(wxCommandEvent&);
    void MenuFileSaveScene(wxCommandEvent&);
    void MenuFileSaveSceneAs(wxCommandEvent&);
    void MenuFileExit(wxCommandEvent&);

    void MenuEditCreateObject(wxCommandEvent&);
    void MenuEditCreateTerrain(wxCommandEvent&);
    void MenuEditTerrainHeightPainter(wxCommandEvent&);
    void MenuEditTerrainTexturePainter(wxCommandEvent&);
    void MenuEditLoadPrefab(wxCommandEvent&);
    void MenuEditClearScene(wxCommandEvent&);

    void MenuRendererReloadShaders(wxCommandEvent&);
    void MenuRendererTakeSnapshot(wxCommandEvent&);
    void MenuRendererSwap(wxCommandEvent&);
    void MenuRendererPhysicsVisualization(wxCommandEvent&);
    void MenuRendererNormalsVisualization(wxCommandEvent&);
    void MenuRendererTextureDiffuse(wxCommandEvent&);
    void MenuRendererTextureNormals(wxCommandEvent&);
    void MenuRendererTextureSpecular(wxCommandEvent&);
    void MenuRendererTextureDisplacement(wxCommandEvent&);

    void OnObjectTreeSelChange(wxTreeEvent&);
    void OnObjectTreeActivated(wxTreeEvent&);
    void OnObjectDrag(wxTreeEvent&);
    void OnObjectEndDrag(wxTreeEvent&);

    void OnClose(wxCloseEvent&);

    void OnGLVersion(wxCommandEvent&);

    void OnFileSelectChanged(wxTreeEvent&);
    void OnFileActivated(wxTreeEvent&);

    void AddChilds(GameEngine::GameObject&, wxTreeItemId);
    void CreateRootGameObject();

    void CreateMainMenu();
    wxMenu* CreateFileMenu();
    wxMenu* CreateEditMenu();
    wxMenu* CreateRendererMenu();
    wxMenu* CreateAboutMenu();

    GameEngine::GameObject* AddGameObject(const std::string& = "NewGameObject", IdType = 0);
    wxTreeItemId AddGameObjectToWxWidgets(wxTreeItemId, IdType, const std::string&);

    void UpdateObjectCount();
    GameEngine::GameObject* GetSelectedGameObject();
    GameEngine::GameObject* GetGameObject(wxTreeItemId);
    void ChangeGameObjectParent(GameEngine::GameObject& object, GameEngine::GameObject& newParent);

private:
    GLCanvas* canvas;
    wxTreeCtrl* gameObjectsView;
    wxTreeItemId treeRootId;
    wxTreeItemId treeDragItemId;
    std::unordered_map<wxTreeItemId, IdType, TreeItemIdHasher, TreeItemIdEqual> gameObjectsItemsIdsMap;
    wxGenericDirCtrl* fileExplorer;
    std::thread loadSceneThread;
    wxDECLARE_EVENT_TABLE();
    bool isRunning{true};
};
