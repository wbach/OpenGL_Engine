
#pragma once
#include <GameEngine/Renderers/DebugElements/DebugRenderer.h>
#include <Types.h>
#include <wx/frame.h>
#include <wx/generic/dirctrlg.h>
#include <wx/notebook.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

#include <thread>
#include <unordered_map>

#include "GameEngine/DebugTools/Painter/Painter.h"
#include "TransformPanel.h"

class GLCanvas;
class OptionsFrame;

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

class TransfromSubController
{
public:
    using GameObjectId = IdType;

    enum State
    {
        world = 0,
        local,
    };
    TransfromSubController(GLCanvas&, TransformPanel*, TransformPanel*, GameObjectId);
    ~TransfromSubController();
    void ChangeGameObject(GameObjectId);
    void ChangeState(State);

private:
    void SubscribeCurrent();
    void UnsubscribeCurrent();

private:
    GLCanvas& canvas;
    std::vector<TransformPanel*> transformPanels;
    std::optional<IdType> subId;
    IdType gameObjectId;
    State state;
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
    void MenuEditPreferences(wxCommandEvent&);

    void MenuRendererReloadShaders(wxCommandEvent&);
    void MenuRendererTakeSnapshot(wxCommandEvent&);
    void MenuRendererSwap(wxCommandEvent&);
    void MenuRendererPhysicsVisualization(wxCommandEvent&);
    void MenuRendererNormalsVisualization(wxCommandEvent&);
    void MenuRendererTextureDiffuse(wxCommandEvent&);
    void MenuRendererTextureNormals(wxCommandEvent&);
    void MenuRendererTextureSpecular(wxCommandEvent&);
    void MenuRendererTextureDisplacement(wxCommandEvent&);

    void SetDeubgRendererState(GameEngine::DebugRenderer::RenderState);

    void OnObjectTreeSelChange(wxTreeEvent&);
    void OnObjectTreeActivated(wxTreeEvent&);
    void OnTreeItemRightClick(wxTreeEvent&);
    void OnBeginLabelEdit(wxTreeEvent&);
    void OnEndLabelEdit(wxTreeEvent&);
    void OnAddObject(wxCommandEvent&);
    void OnDeleteObject(wxCommandEvent&);
    void CloneGameObject(wxCommandEvent&);
    void OnObjectDrag(wxTreeEvent&);
    void OnObjectEndDrag(wxTreeEvent&);
    void OnPageChanged(wxNotebookEvent&);

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
    std::optional<IdType> GetGameObjectId(wxTreeItemId);
    void ChangeGameObjectParent(GameEngine::GameObject& object, GameEngine::GameObject& newParent);
    GameEngine::Painter::EntryParamters GetPainterEntryParameters();
    void AddGameObjectComponentsToView(const GameEngine::GameObject&);
    void RemoveAllComponentPanels();

private:
    GLCanvas* canvas{nullptr};
    OptionsFrame* optionsFrame{nullptr};
    wxTreeCtrl* gameObjectsView{nullptr};
    TransformPanel* worldTransformPanel{nullptr};
    TransformPanel* localTransformPanel{nullptr};
    wxGenericDirCtrl* fileExplorer{nullptr};
    wxBoxSizer* gameObjectPanelsSizer{nullptr};
    wxScrolledWindow* gameObjectPanels{nullptr};
    wxButton* addComponentButton{nullptr};
    wxComboBox* addComponentChoice{nullptr};

    wxTreeItemId treeRootId;
    wxTreeItemId treeDragItemId;
    wxTreeItemId treeRightClickedItem;
    std::unordered_map<wxTreeItemId, IdType, TreeItemIdHasher, TreeItemIdEqual> gameObjectsItemsIdsMap;

    std::thread loadSceneThread;
    wxDECLARE_EVENT_TABLE();
    bool isRunning{true};

    std::optional<TransfromSubController> transfromSubController;
    std::unique_ptr<GameEngine::Painter> terrainPainter_;
};
