
#pragma once
#include <GameEngine/Renderers/DebugElements/DebugRenderer.h>
#include <Types.h>
#include <wx/frame.h>
#include <wx/generic/dirctrlg.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

#include <optional>
#include <string>
#include <thread>
#include <unordered_map>

#include "DisableHelper.h"
#include "GameEngine/DebugTools/Painter/Painter.h"
#include "MyEvents.h"
#include "SceneTreeCtrl.h"
#include "TransformPanel.h"

class GLCanvas;
class OptionsFrame;
class ProjectPanel;

namespace GameEngine
{
class GameObject;
}

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
    void Init();

private:
    void MenuFileNewProject(wxCommandEvent&);
    void MenuFileOpenProject(wxCommandEvent&);
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

    void MenuComponentsRebuild(wxCommandEvent&);
    void MenuComponentsReload(wxCommandEvent&);

    void SetDeubgRendererState(GameEngine::DebugRenderer::RenderState);

    void OnObjectTreeSelChange(wxTreeEvent&);
    void OnObjectTreeActivated(wxTreeEvent&);
    void OnBeginLabelEdit(wxTreeEvent&);
    void OnEndLabelEdit(wxTreeEvent&);
    void OnAddObject(wxCommandEvent&);
    void OnDeleteObject(wxCommandEvent&);
    void OnUnmarkPrefab(wxCommandEvent&);
    void OnMakePrefab(wxCommandEvent&);
    void OnRename(wxCommandEvent&);
    void CloneGameObject(wxCommandEvent&);
    void OnPageChanged(wxNotebookEvent&);

    void OnClose(wxCloseEvent&);

    void OnGLVersion(wxCommandEvent&);

    void OnFileSelectChanged(wxTreeEvent&);
    void OnFileActivated(const wxString&);

    void AddChilds(GameEngine::GameObject&, wxTreeItemId);

    void CreateMainMenu();
    wxMenu* CreateFileMenu();
    wxMenu* CreateEditMenu();
    wxMenu* CreateRendererMenu();
    wxMenu* CreateComponentsMenu();
    wxMenu* CreateAboutMenu();

    void CreateToolBarForEngine();
    void UpdateTimeOnToolbar();

    GameEngine::GameObject* AddGameObject(const std::string& = "NewGameObject", IdType = 0);

    void UpdateObjectCount();
    GameEngine::GameObject* GetSelectedGameObject();
    GameEngine::GameObject* GetGameObject(wxTreeItemId);
    void ChangeGameObjectParent(GameEngine::GameObject& object, GameEngine::GameObject& newParent);
    GameEngine::Painter::EntryParamters GetPainterEntryParameters();
    void AddGameObjectComponentsToView(GameEngine::GameObject&);
    void LockAllComponentPanels();
    void UnlockAllComponentPanels();
    void RemoveAllComponentPanels();
    void ClearScene();
    void OnToolStart(wxCommandEvent&);
    void OnToolStop(wxCommandEvent&);
    void OnToolAnimationViewer(wxCommandEvent&);
    void OnBuildCmponents(wxCommandEvent&);

    void RunCommand(const std::string& cmd, const std::string& workDir, wxProcess *process = nullptr);

    bool SaveSceneAs();
    void SaveSceneAs(const std::string&);
    void UpdateGameObjectIdOnTransfromLabel(std::optional<IdType> = std::nullopt);
    void LoadPrefab(const std::string&);

private:
    GLCanvas* canvas{nullptr};
    OptionsFrame* optionsFrame{nullptr};
    ProjectPanel* projectPanel{nullptr};
    std::unique_ptr<SceneTreeCtrl> gameObjectsView{nullptr};
    wxCollapsiblePane* transformsCollapsible{nullptr};
    TransformPanel* worldTransformPanel{nullptr};
    TransformPanel* localTransformPanel{nullptr};
    wxBoxSizer* gameObjectPanelsSizer{nullptr};
    wxScrolledWindow* gameObjectPanels{nullptr};
    wxButton* addComponentButton{nullptr};
    wxComboBox* addComponentChoice{nullptr};

    wxSlider* timeSlider{nullptr};
    wxSpinCtrl* hourCtrl{nullptr};
    wxSpinCtrl* minuteCtrl{nullptr};

    std::thread loadSceneThread;
    wxDECLARE_EVENT_TABLE();
    bool isRunning{true};

    std::optional<TransfromSubController> transfromSubController;
    std::unique_ptr<GameEngine::Painter> terrainPainter_;

    std::optional<long> startedGameProceesId;
    std::optional<IdType> sceneEventSubId;
};
