#include "MainFrame.h"

#include <GameEngine/Components/FunctionType.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Resources/Models/Material.h>
#include <GameEngine/Scene/SceneEvents.h>
#include <GameEngine/Scene/SceneReader.h>
#include <GameEngine/Scene/SceneUtils.h>
#include <Logger/Log.h>
#include <Utils/XML/XMLUtils.h>
#include <wx/artprov.h>
#include <wx/defs.h>
#include <wx/dnd.h>
#include <wx/filedlg.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/splitter.h>
#include <wx/statbmp.h>
#include <wx/stdpaths.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>
#include <magic_enum/magic_enum.hpp>
#include <string>

#include "AnimationViewer/AnimationViwerIcon.h"
#include "ComponentPanel/BuildComponentLogFrame.h"
#include "ComponentPanel/BuildProcess.h"
#include "ComponentPanel/ComponentPanel.h"
#include "ComponentPanel/ComponentPickerPopup.h"
#include "ComponentPanel/TransformPanel.h"
#include "ControlsIds.h"
#include "OptionsFrame/OptionsFrame.h"
#include "OptionsFrame/Theme.h"
#include "ProjectManager.h"
#include "ProjectPanel/ProjectPanel.h"
#include "TerrainTool/TerrainToolPanel.h"
#include "WxEditor/EngineRelated/GLCanvas.h"
#include "WxEditor/WxHelpers/EditorUitls.h"
#include "WxEditor/WxHelpers/LoadingDialog.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>

#include <csignal>
#endif

namespace
{
class GLCanvasDropTarget : public wxFileDropTarget
{
public:
    using Callback = std::function<void(const wxString&)>;
    GLCanvasDropTarget(Callback callback)
        : callback{callback}
    {
    }

    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override
    {
        LOG_DEBUG << "Drop file" << filenames[0];
        if (not filenames.IsEmpty())
        {
            if (callback)
            {
                callback(filenames[0]);
                return true;
            }
        }
        return false;
    }

private:
    Callback callback;
};

bool terminateProcessByPID(long pid)
{
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(pid));
    if (!hProcess)
    {
        std::cerr << "Nie mozna otworzyc procesu, PID=" << pid << "\n";
        return false;
    }

    if (!TerminateProcess(hProcess, 0))
    {
        std::cerr << "Nie udalo sie zakonczyc procesu, PID=" << pid << "\n";
        CloseHandle(hProcess);
        return false;
    }

    CloseHandle(hProcess);
    return true;

#else
    if (kill(static_cast<pid_t>(pid), SIGTERM) != 0)
    {
        perror("Nie udalo sie zakonczyc procesu");
        return false;
    }
    return true;
#endif
}
}  // namespace

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_MENU(ID_MENU_FILE_NEW_SCENE, MainFrame::MenuFileNewScene)
    EVT_MENU(ID_MENU_FILE_OPEN_SCENE, MainFrame::MenuFileOpenScene)
    EVT_MENU(ID_MENU_FILE_RELOAD_SCENE, MainFrame::MenuFileReloadScene)
    EVT_MENU(ID_MENU_FILE_SAVE_SCENE, MainFrame::MenuFileSaveScene)
    EVT_MENU(ID_MENU_FILE_SAVEAS_SCENE, MainFrame::MenuFileSaveSceneAs)
    EVT_MENU(ID_MENU_FILE_EXIT, MainFrame::MenuFileExit)
    EVT_MENU(ID_MENU_EDIT_CREATE_OBJECT, MainFrame::MenuEditCreateObject)
    EVT_MENU(ID_MENU_EDIT_CREATE_TERRAIN, MainFrame::MenuEditCreateTerrain)
    EVT_MENU(ID_MENU_EDIT_MATERIAL_EDITOR, MainFrame::MenuEditMaterialEditor)
    EVT_MENU(ID_MENU_EDIT_LOAD_PREFAB, MainFrame::MenuEditLoadPrefab)
    EVT_MENU(ID_MENU_EDIT_CLEAR_SCENE, MainFrame::MenuEditClearScene)
    EVT_MENU(ID_MENU_EDIT_PREFERENCES, MainFrame::MenuEditPreferences)
    EVT_MENU(ID_MENU_RENDERER_RELOAD_SHADERS, MainFrame::MenuRendererReloadShaders)
    EVT_MENU(ID_MENU_RENDERER_TAKE_RENDERER_SNAPSHOT, MainFrame::MenuRendererTakeSnapshot)
    EVT_MENU(ID_MENU_RENDERER_SWAP, MainFrame::MenuRendererSwap)
    EVT_MENU(ID_MENU_RENDERER_PHYSICS_VISUALIZATION, MainFrame::MenuRendererPhysicsVisualization)
    EVT_MENU(ID_MENU_RENDERER_BOUNDING_BOX_VISUALIZATION, MainFrame::MenuRendererBoundingBoxVisualization)
    EVT_MENU(ID_MENU_RENDERER_NORMAL_VISUALIZATION, MainFrame::MenuRendererNormalsVisualization)
    EVT_MENU(ID_MENU_RENDERER_TEXTURE_AMBIENT, MainFrame::MenuRendererTextureAmbient)
    EVT_MENU(ID_MENU_RENDERER_TEXTURE_DIFFUSE, MainFrame::MenuRendererTextureDiffuse)
    EVT_MENU(ID_MENU_RENDERER_TEXTURE_NORMALS, MainFrame::MenuRendererTextureNormals)
    EVT_MENU(ID_MENU_RENDERER_TEXTURE_SPECULAR, MainFrame::MenuRendererTextureSpecular)
    EVT_MENU(ID_MENU_EDIT_CLEAR_SCENE, MainFrame::MenuRendererTextureDisplacement)
    EVT_MENU(ID_MENU_ABOUT_GL_INFO, MainFrame::OnGLVersion)
    EVT_MENU(ID_MENU_COMPONENTS_REBUILD, MainFrame::MenuComponentsRebuild)
    EVT_MENU(ID_MENU_COMPONENTS_RELOAD, MainFrame::MenuComponentsReload)
    EVT_TREE_SEL_CHANGED(ID_OBJECT_TREE, MainFrame::OnObjectTreeSelChange)
    EVT_TREE_ITEM_ACTIVATED(ID_OBJECT_TREE, MainFrame::OnObjectTreeActivated)
    EVT_TREE_BEGIN_LABEL_EDIT(ID_OBJECT_TREE, MainFrame::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(ID_OBJECT_TREE, MainFrame::OnEndLabelEdit)
    EVT_DIRCTRL_SELECTIONCHANGED(ID_FILE_EXPLORER, MainFrame::OnFileSelectChanged)
    EVT_MENU(ID_TOOL_OPEN, MainFrame::MenuFileOpenScene)
    EVT_MENU(ID_TOOL_SAVE, MainFrame::MenuFileSaveScene)
    EVT_MENU(ID_TOOL_SAVE_AS, MainFrame::MenuFileSaveSceneAs)
    EVT_MENU(ID_TOOL_START, MainFrame::OnToolStart)
    EVT_MENU(ID_TOOL_STOP, MainFrame::OnToolStop)
    EVT_MENU(ID_TOOL_ANIMATION_VIEWER, MainFrame::OnToolAnimationViewer)
    EVT_MENU(ID_TOOL_BUILD, MainFrame::OnBuildCmponents)
    EVT_MENU(ID_TOOL_TERRAIN_PANEL, MainFrame::OnToggleTerrainPanel)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
// clang-format on
{
    // Init();
    LOG_DEBUG << "xx";
}

MainFrame::~MainFrame()
{
}

void MainFrame::Init()
{
    wxInitAllImageHandlers();

    // Glowny splitter: lewy/prawy
    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY);

    // Lewy splitter: gora/dol
    wxSplitterWindow* leftSplitter = new wxSplitterWindow(mainSplitter, wxID_ANY);

    // Gora lewego splittera: tree + canvas (+ boczny panel)
    wxSplitterWindow* topSplitter = new wxSplitterWindow(leftSplitter, wxID_ANY);

    // === Tree ===
    auto treeCtrl   = new wxTreeCtrl(topSplitter, ID_OBJECT_TREE, wxPoint(0, 0), wxSize(160, 250),
                                     wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_EDIT_LABELS);
    gameObjectsView = std::make_unique<SceneTreeCtrl>(treeCtrl,
                                                      [this](IdType item, IdType newParent)
                                                      {
                                                          auto& scene      = canvas->GetScene();
                                                          auto go          = scene.GetGameObject(item);
                                                          auto newParentGo = scene.GetGameObject(newParent);
                                                          if (go and newParentGo)
                                                          {
                                                              ChangeGameObjectParent(*go, *newParentGo);
                                                          }
                                                      });

    Bind(wxEVT_MENU, &MainFrame::OnAddObject, this, ID_TREE_MENU_CREATE_CHILD);
    Bind(wxEVT_MENU, &MainFrame::OnUnmarkPrefab, this, ID_TREE_MENU_UNMARK_PREFAB);
    Bind(wxEVT_MENU, &MainFrame::OnMakePrefab, this, ID_TREE_MENU_MAKE_PREFAB);
    Bind(wxEVT_MENU, &MainFrame::OnDeleteObject, this, ID_TREE_MENU_REMOVE);
    Bind(wxEVT_MENU, &MainFrame::OnRename, this, ID_TREE_MENU_RENAME);
    Bind(wxEVT_MENU, &MainFrame::CloneGameObject, this, ID_TREE_MENU_CLONE);

    auto onStartupDone = [this]()
    {
        UpdateTimeOnToolbar();
        gameObjectsView->SubscribeForSceneEvent(canvas->GetScene());
        UpdateMainMenuRendererOptionsCheckBoxes();
        SetStatusText(std::filesystem::path(EngineConf.files.getDataPath()).make_preferred().string());
    };
    auto selectItemInGameObjectTree = [this](uint32 gameObjectId, bool select)
    {
        if (auto wxItemId = gameObjectsView->Get(gameObjectId))
        {
            gameObjectsView->SelectItem(*wxItemId, select);
            UpdateGameObjectIdOnTransfromLabel(gameObjectId);
        }
    };

    // === Prawy splitter: canvas + panel boczny ===
    rightSplitter = new wxSplitterWindow(topSplitter, wxID_ANY);

    // GLCanvas -- UWAGA: parent to rightSplitter (nie topSplitter!)
    canvas = new GLCanvas(rightSplitter, onStartupDone, selectItemInGameObjectTree);
    canvas->SetDropTarget(new GLCanvasDropTarget([this](const auto& file) { OnFileActivated(file); }));

    // Panel boczny obok canvas dla terrain tools
    rightSplitter->Initialize(canvas);
    rightSplitter->SetSashGravity(1.0);
    rightSplitter->SetMinimumPaneSize(300);
    terrainPanelVisible = false;

    auto size = GetSize();

    // Split pionowy: tree (lewa) + rightSplitter (canvas + panel)
    topSplitter->SplitVertically(gameObjectsView->GetWxTreeCtrl(), rightSplitter, size.x / 8);

    // === Dol: ProjectPanel ===
    auto fileSelectedCallback = [this](const wxString& str) { OnFileActivated(str); };
    projectPanel = new ProjectPanel(leftSplitter, ProjectManager::GetInstance().GetProjectPath().string(), fileSelectedCallback);

    // Lewy splitter: gora (tree+canvas+panel), dol (projectPanel)
    leftSplitter->SplitHorizontally(topSplitter, projectPanel, size.y * 3 / 5);

    // === Prawa strona: gameObjectPanels ===
    gameObjectPanels = new wxScrolledWindow(mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    gameObjectPanels->SetScrollRate(5, 5);

    gameObjectPanelsSizer = new wxBoxSizer(wxVERTICAL);
    gameObjectPanels->SetSizer(gameObjectPanelsSizer);

    // Tworzymy collapsible, ktory bedzie "kontenerem" dla notebooka
    transformsCollapsible = new wxCollapsiblePane(gameObjectPanels, wxID_ANY, "Transform");
    UpdateGameObjectIdOnTransfromLabel();

    gameObjectPanelsSizer->Add(transformsCollapsible, 0, wxEXPAND | wxALL, 0);

    wxWindow* pane = transformsCollapsible->GetPane();

    auto transformsNotebook = new wxNotebook(pane, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    transformsNotebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &MainFrame::OnPageChanged, this);

    worldTransformPanel = new TransformPanel(transformsNotebook);
    transformsNotebook->AddPage(worldTransformPanel, "World");

    localTransformPanel = new TransformPanel(transformsNotebook);
    transformsNotebook->AddPage(localTransformPanel, "Local");

    worldTransformPanel->lock();
    localTransformPanel->lock();

    wxBoxSizer* paneSizer = new wxBoxSizer(wxVERTICAL);
    paneSizer->Add(transformsNotebook, 1, wxEXPAND | wxALL, 0);
    pane->SetSizer(paneSizer);

    gameObjectPanels->Layout();
    gameObjectPanels->FitInside();

    transformsCollapsible->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED,
                                [this](wxCollapsiblePaneEvent&)
                                {
                                    gameObjectPanelsSizer->Layout();
                                    gameObjectPanels->FitInside();
                                    gameObjectPanels->Refresh();
                                });

    transformsCollapsible->Collapse(false);

    // Main split: lewa/prawa
    mainSplitter->SplitVertically(leftSplitter, gameObjectPanels, 3 * size.x / 4);

    // === Reszta GUI ===
    CreateMainMenu();
    CreateToolBarForEngine();

    CreateStatusBar();
    SetStatusText("Welcome to game editor!");

    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, (int)'S', ID_SAVE);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    // Powiazanie zdarzenia z ID_SAVE
    Bind(wxEVT_MENU, &MainFrame::MenuFileSaveScene, this, ID_SAVE);
    SaveOsTheme(*this);

    Bind(wxEVT_RELOAD_COMPONENT_LIB_EVENT,
         [this](ReloadComponentLibEvent& event)
         {
             LOG_DEBUG << "wxEVT_RELOAD_COMPONENT_LIB_EVENT";
             RemoveAllComponentPanels();
             canvas->GetEngine().getExternalComponentsReader().Reload(event.GetFile());
             AddGameObjectComponentsToView(event.GetGameObject());
         });
}

void MainFrame::LockAllComponentPanels()
{
    if (!gameObjectPanelsSizer)
        return;

    for (int i = gameObjectPanelsSizer->GetItemCount() - 1; i >= 0; --i)
    {
        wxSizerItem* item = gameObjectPanelsSizer->GetItem(i);
        if (!item)
            continue;

        wxWindow* win = item->GetWindow();
        if (win)
        {
            if (auto componentPanel = dynamic_cast<ComponentPanel*>(win))
            {
                componentPanel->Lock();
            }
        }
    }
}

void MainFrame::UnlockAllComponentPanels()
{
    if (!gameObjectPanelsSizer)
        return;

    for (int i = gameObjectPanelsSizer->GetItemCount() - 1; i >= 0; --i)
    {
        wxSizerItem* item = gameObjectPanelsSizer->GetItem(i);
        if (!item)
            continue;

        wxWindow* win = item->GetWindow();
        if (win)
        {
            if (auto componentPanel = dynamic_cast<ComponentPanel*>(win))
            {
                componentPanel->Unlock();
            }
        }
    }
}

void MainFrame::RemoveAllComponentPanels()
{
    if (!gameObjectPanelsSizer)
        return;

    for (int i = gameObjectPanelsSizer->GetItemCount() - 1; i >= 0; --i)
    {
        wxSizerItem* item = gameObjectPanelsSizer->GetItem(i);
        if (!item)
            continue;

        wxWindow* win = item->GetWindow();
        if (win && (dynamic_cast<ComponentPanel*>(win)))
        {
            gameObjectPanelsSizer->Detach(i);
            win->Destroy();
        }
    }
    gameObjectPanelsSizer->Layout();
}

void MainFrame::ClearScene()
{
    if (terrainPanel)
    {
        rightSplitter->Unsplit(terrainPanel);
        terrainPanelVisible = false;
        canvas->EnablePicker();

        terrainPanel->Destroy();
        terrainPanel = nullptr;

        rightSplitter->Layout();
    }

    canvas->GetScene().ClearGameObjects();
    gameObjectsView->DeleteAllItems();
    canvas->ResetDragObject();
    transfromSubController.reset();
    RemoveAllComponentPanels();
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    if (terrainPanel)
    {
        terrainPanel->Cleanup();
    }

    LOG_DEBUG << "OnClose";
    isRunning = false;
    if (loadSceneThread.joinable())
    {
        loadSceneThread.join();
    }
    event.Skip();
}

void MainFrame::MenuFileNewScene(wxCommandEvent&)
{
    if (canvas->GetScene().GetGameObjects().size() > 0)
    {
        int answer = wxMessageBox("Current scene not empty, close this scene and create new one?", "Confirmation",
                                  wxYES_NO | wxICON_QUESTION);

        if (answer != wxYES)
        {
            return;
        }
    }

    ClearScene();
    canvas->CreateNewScene();
    SetTitle("New scene: not saved");
}

void MainFrame::MenuFileOpenScene(wxCommandEvent&)
{
    if (canvas->GetScene().GetGameObjects().size() > 0)
    {
        int answer = wxMessageBox("Current scene not empty, close this scene and open new one?", "Confirmation",
                                  wxYES_NO | wxICON_QUESTION);

        if (answer != wxYES)
        {
            return;
        }
    }

    wxFileDialog openFileDialog(this, "Wybierz plik", ProjectManager::GetInstance().GetScenesDir().string(), "",
                                "Pliki sceny (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString path = openFileDialog.GetPath();
    GameEngine::File file{std::string{path.c_str()}};

    auto dlg = std::make_shared<LoadingDialog>(this, "Open scene", "Loading " + file.GetBaseName());

    ClearScene();
    gameObjectsView->UnSubscribeForSceneEvent();

    canvas->OpenScene(file,
                      [&, path = file.GetDataRelativePath().string(), loadingDialog = dlg]()
                      {
                          if (isRunning)
                          {
                              LOG_DEBUG << "Scene loaded callback";
                              SetStatusText("Welcome to game editor!");
                              SetTitle("Active scene : " + canvas->GetScene().GetName());
                              canvas->GetScene().GetFile() = path;
                              this->CallAfter(
                                  [&]()
                                  {
                                      gameObjectsView->RebuildTree(canvas->GetScene());
                                      gameObjectsView->SubscribeForSceneEvent(canvas->GetScene());
                                      dlg->EndModal(wxID_OK);
                                  });
                          }
                      });
    SetStatusText("Loading file " + file.GetBaseName());
    dlg->ShowModal();
}

void MainFrame::MenuFileReloadScene(wxCommandEvent&)
{
    SetStatusText("Reloding scene " + canvas->GetScene().GetName());
    ClearScene();
    gameObjectsView->UnSubscribeForSceneEvent();
    canvas->GetEngine().GetEngineContext().AddEngineEvent(
        GameEngine::ChangeSceneEvent{GameEngine::ChangeSceneEvent::Type::RELOAD_SCENE});
}

void MainFrame::MenuFileSaveScene(wxCommandEvent&)
{
    if (not canvas->GetScene().GetFile().empty())
    {
        GameEngine::saveSceneToFile(canvas->GetScene());
        return;
    }

    SaveSceneAs();
}

void MainFrame::MenuFileSaveSceneAs(wxCommandEvent&)
{
    SaveSceneAs();
}

void MainFrame::MenuFileExit(wxCommandEvent&)
{
    Close(true);
}

void MainFrame::MenuEditCreateObject(wxCommandEvent&)
{
    auto dlg = createEntryDialogWithSelectedText(this, "Enter object name:", "Crete new game object", "NewGameObject",
                                                 wxOK | wxCANCEL | wxCENTRE);

    if (dlg->ShowModal() == wxID_CANCEL)
        return;

    auto parentGameObjectId = canvas->GetScene().GetRootGameObject().GetId();
    if (auto parentGameObject = GetSelectedGameObject())
    {
        parentGameObjectId = parentGameObject->GetId();
    }
    AddGameObject(dlg->GetValue().IsEmpty() ? "NewGameObject" : dlg->GetValue().ToStdString(), parentGameObjectId);
}

GameEngine::GameObject* MainFrame::AddGameObject(const std::string& name, IdType parentId)
{
    auto& scene           = canvas->GetScene();
    auto gameObject       = scene.CreateGameObject(name);
    auto result           = gameObject.get();
    auto parentGameObject = scene.GetGameObject(parentId);

    if (parentGameObject)
    {
        auto go            = gameObject.get();
        auto worldPosition = gameObject->GetWorldTransform().GetPosition();
        auto worldRotation = gameObject->GetWorldTransform().GetRotation();
        auto worldScale    = gameObject->GetWorldTransform().GetScale();
        LOG_DEBUG << "NewGameObj add: " << name;

        scene.AddGameObject(*parentGameObject, std::move(gameObject));
        scene.SendEvent(
            GameEngine::ModifyGameObjectEvent{.gameObjectId   = go->GetId(),
                                              .worldTransform = GameEngine::ModifyGameObjectEvent::SetTransform{
                                                  .position = worldPosition, .rotation = worldRotation, .scale = worldScale}});
    }
    else
    {
        LOG_DEBUG << ("NewGameObj add");
        canvas->GetScene().AddGameObject(std::move(gameObject));
    }

    gameObjectsView->SelectItemWhenGameObjectBecomeAvaiable(result->GetId());

    return result;
}

GameEngine::GameObject* MainFrame::GetSelectedGameObject()
{
    auto treeSelectedItemId = gameObjectsView->GetSelection();
    if (not treeSelectedItemId.IsOk())
    {
        return &canvas->GetScene().GetRootGameObject();
    }

    return GetGameObject(treeSelectedItemId);
}

GameEngine::GameObject* MainFrame::GetGameObject(wxTreeItemId id)
{
    if (auto goId = gameObjectsView->Get(id))
    {
        return canvas->GetScene().GetGameObject(*goId);
    }
    return nullptr;
}

void MainFrame::MenuEditCreateTerrain(wxCommandEvent&)
{
    auto dlg = createEntryDialogWithSelectedText(this, "Enter terrain name:", "Crete new terrain object", "MyTerrain",
                                                 wxOK | wxCANCEL | wxCENTRE);

    if (dlg->ShowModal() == wxID_CANCEL)
        return;

    auto newTerrainGo =
        canvas->GetScene().CreateGameObject(dlg->GetValue().IsEmpty() ? "Terrain" : dlg->GetValue().ToStdString());
    auto& tc = newTerrainGo->AddComponent<GameEngine::Components::TerrainRendererComponent>();
    tc.createHeightMap();
    canvas->GetScene().AddGameObject(std::move(newTerrainGo));
}

void MainFrame::MenuEditCreateMaterial(wxCommandEvent&)
{
    wxFileDialog fileDialog(this, "Wybierz plik", ProjectManager::GetInstance().GetDataDir().string(), "",
                            "Pliki materialow (*.json)|*.json", wxFD_SAVE);

    if (fileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString path = fileDialog.GetPath();
    GameEngine::SaveMaterial(GameEngine::Material{}, path.ToStdString());
}

void MainFrame::AddGameObjectComponentsToView(GameEngine::GameObject& gameObject)
{
    for (auto& [_, vectorOfComponents] : gameObject.GetComponents())
    {
        for (auto& component : vectorOfComponents)
        {
            auto* compPanel =
                new ComponentPanel(this, gameObjectPanels, canvas->GetEngine().getExternalComponentsReader(), gameObject);
            compPanel->AddComponent(*component);
            if (isGameObjectPrefab(gameObject))
            {
                compPanel->Lock();
            }
            gameObjectPanelsSizer->Add(compPanel, 0, wxEXPAND | wxALL, 0);
        }
    }

    if (addComponentButton)
    {
        addComponentButton->Destroy();
    }

    addComponentButton = new wxButton(gameObjectPanels, wxID_ANY, "Add component");
    gameObjectPanelsSizer->Add(addComponentButton, 0, wxEXPAND | wxALL, 0);

    auto action = [&](wxCommandEvent&)
    {
        auto popup = new ComponentPickerPopup(
            gameObjectPanels, canvas->GetScene().getComponentController(), gameObject,
            [this, &gameObject](auto& component)
            {
                ComponentPanel* compPanel =
                    new ComponentPanel(this, gameObjectPanels, canvas->GetEngine().getExternalComponentsReader(), gameObject);
                compPanel->AddComponent(component, false);
                canvas->GetScene().getComponentController().CallGameObjectFunctions(GameEngine::Components::FunctionType::Awake,
                                                                                    gameObject.GetId());
                canvas->GetScene().getComponentController().CallGameObjectFunctions(
                    GameEngine::Components::FunctionType::LateAwake, gameObject.GetId());

                if (isGameObjectPrefab(gameObject))
                {
                    compPanel->Lock();
                }

                this->CallAfter(
                    [this, compPanel]()
                    {
                        int btnIndex = gameObjectPanelsSizer->GetItemCount() - 1;
                        if (btnIndex < 0)
                            btnIndex = 0;  // zabezpieczenie, gdyby przycisku jeszcze nie bylo
                        gameObjectPanelsSizer->Insert(btnIndex, compPanel, 0, wxEXPAND | wxALL, 0);

                        gameObjectPanelsSizer->Layout();
                        gameObjectPanels->FitInside();
                        gameObjectPanels->Refresh();
                    });
            });

        // Pobieramy pozycje przycisku w globalnych wspolrzednych
        wxPoint pos = addComponentButton->GetScreenPosition();

        // Ustawiamy szerokosc popupu na szerokosc przycisku
        int buttonWidth = addComponentButton->GetSize().GetWidth();
        wxSize popupSize(buttonWidth, 2 * popup->GetSize().GetHeight());
        popup->SetSize(popupSize);

        // Pozycjonujemy i pokazujemy popup
        // popup->Position(pos, wxSize(0, 0));
        popup->Popup(pos);
    };
    addComponentButton->Bind(wxEVT_BUTTON, action);

    // Odswiezenie layoutu po dodaniu przycisku
    gameObjectPanelsSizer->Layout();
    gameObjectPanels->FitInside();
    gameObjectPanels->Refresh();
}

void MainFrame::MenuEditMaterialEditor(wxCommandEvent&)
{
    runMaterialEditor();
}

void MainFrame::MenuEditLoadPrefab(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, "Wybierz plik", ProjectManager::GetInstance().GetProjectPath().string(), "",
                                "Pliki prefab (*.prefab)|*.prefab|Pliki prefab (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;
    LoadPrefab(openFileDialog.GetPath().ToStdString());
}

void MainFrame::LoadPrefab(const std::string& path)
{
    if (auto go = GameEngine::SceneReader(canvas->GetScene()).loadPrefab(path))
    {
        gameObjectsView->SelectItemWhenGameObjectBecomeAvaiable(go->GetId());
    }
}

void MainFrame::MenuEditClearScene(wxCommandEvent&)
{
    int answer = wxMessageBox("Please confirm clear scene", "Confirmation", wxYES_NO | wxICON_QUESTION);

    if (answer == wxYES)
    {
        ClearScene();
    }
}

void MainFrame::MenuEditPreferences(wxCommandEvent&)
{
    if (not optionsFrame)
    {
        optionsFrame = new OptionsFrame(this);
    }

    optionsFrame->Show();
}

void MainFrame::MenuRendererReloadShaders(wxCommandEvent&)
{
    auto& renderesManager = canvas->GetEngine().GetEngineContext().GetRenderersManager();
    renderesManager.ReloadShaders();
}

void MainFrame::MenuRendererTakeSnapshot(wxCommandEvent&)
{
    std::string path{"./snapshoot/"};
    auto& resourceManager = canvas->GetScene().GetResourceManager();
    auto takeSnapshoot    = [&, path]() { resourceManager.GetGraphicsApi().TakeSnapshoot(path); };
    resourceManager.GetGpuResourceLoader().AddFunctionToCall(takeSnapshoot);
}

void MainFrame::MenuRendererSwap(wxCommandEvent&)
{
    auto& renderesManager = canvas->GetEngine().GetEngineContext().GetRenderersManager();
    renderesManager.setLineRenderMode(!renderesManager.getLineRenderMode());
}

void MainFrame::MenuRendererPhysicsVisualization(wxCommandEvent&)
{
    LOG_DEBUG << "";
    auto set = SetDeubgRendererState(GameEngine::DebugRenderer::RenderState::Physics);

    auto& papi = canvas->GetEngine().GetEngineContext().GetPhysicsApi();

    if (set)
        papi.enableVisualizationForAllRigidbodys();
    else
        papi.disableVisualizationForAllRigidbodys();

    rendererMenu->Check(ID_MENU_RENDERER_PHYSICS_VISUALIZATION, set);
}

bool MainFrame::SetDeubgRendererState(GameEngine::DebugRenderer::RenderState state)
{
    auto& renderesManager = canvas->GetEngine().GetEngineContext().GetRenderersManager();
    bool set              = renderesManager.GetDebugRenderer().IsStateEnabled(state);
    set                   = !set;

    auto& debugRenderer = renderesManager.GetDebugRenderer();
    set ? debugRenderer.AddState(state) : debugRenderer.RemoveState(state);
    return set;
}

void MainFrame::MenuRendererBoundingBoxVisualization(wxCommandEvent&)
{
    bool set = SetDeubgRendererState(GameEngine::DebugRenderer::RenderState::BoundingBox);
    rendererMenu->Check(ID_MENU_RENDERER_BOUNDING_BOX_VISUALIZATION, set);
}

void MainFrame::MenuRendererNormalsVisualization(wxCommandEvent&)
{
    bool set = SetDeubgRendererState(GameEngine::DebugRenderer::RenderState::Normals);
    rendererMenu->Check(ID_MENU_RENDERER_NORMAL_VISUALIZATION, set);
}

void MainFrame::MenuRendererTextureAmbient(wxCommandEvent&)
{
    auto& textConf      = EngineConf.renderer.textures;
    textConf.useAmbient = !textConf.useAmbient;

    canvas->GetScene().GetResourceManager().GetGpuResourceLoader().AddFunctionToCall(
        [&]() { canvas->GetEngine().GetEngineContext().GetRenderersManager().UpdatePerAppBuffer(); });

    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_AMBIENT, textConf.useAmbient);
}

void MainFrame::MenuRendererTextureDiffuse(wxCommandEvent&)
{
    auto& textConf      = EngineConf.renderer.textures;
    textConf.useDiffuse = !textConf.useDiffuse;

    canvas->GetScene().GetResourceManager().GetGpuResourceLoader().AddFunctionToCall(
        [&]() { canvas->GetEngine().GetEngineContext().GetRenderersManager().UpdatePerAppBuffer(); });

    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_DIFFUSE, textConf.useDiffuse);
}

void MainFrame::MenuRendererTextureNormals(wxCommandEvent&)
{
    auto& textConf     = EngineConf.renderer.textures;
    textConf.useNormal = !textConf.useNormal;

    canvas->GetScene().GetResourceManager().GetGpuResourceLoader().AddFunctionToCall(
        [&]() { canvas->GetEngine().GetEngineContext().GetRenderersManager().UpdatePerAppBuffer(); });

    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_NORMALS, textConf.useNormal);
}

void MainFrame::MenuRendererTextureSpecular(wxCommandEvent&)
{
    auto& textConf       = EngineConf.renderer.textures;
    textConf.useSpecular = !textConf.useSpecular;

    canvas->GetScene().GetResourceManager().GetGpuResourceLoader().AddFunctionToCall(
        [&]() { canvas->GetEngine().GetEngineContext().GetRenderersManager().UpdatePerAppBuffer(); });

    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_SPECULAR, textConf.useSpecular);
}

void MainFrame::MenuRendererTextureDisplacement(wxCommandEvent&)
{
    auto& textConf           = EngineConf.renderer.textures;
    textConf.useDisplacement = !textConf.useDisplacement;

    canvas->GetScene().GetResourceManager().GetGpuResourceLoader().AddFunctionToCall(
        [&]() { canvas->GetEngine().GetEngineContext().GetRenderersManager().UpdatePerAppBuffer(); });

    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_DISPLACEMENT, textConf.useDisplacement);
}

void MainFrame::MenuComponentsRebuild(wxCommandEvent& evt)
{
    OnBuildCmponents(evt);
}

void MainFrame::MenuComponentsReload(wxCommandEvent&)
{
    LOG_DEBUG << "Reloading components...";
    canvas->GetEngine().getExternalComponentsReader().ReloadAll();
    RemoveAllComponentPanels();

    if (auto gameObject = GetSelectedGameObject())
    {
        AddGameObjectComponentsToView(*gameObject);
    }
}

void MainFrame::OnGLVersion(wxCommandEvent&)
{
    wxLogMessage(canvas->getGlInfo().c_str());
}

void MainFrame::CreateMainMenu()
{
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(CreateFileMenu(), "&File");
    menuBar->Append(CreateEditMenu(), "&Edit");
    menuBar->Append(CreateRendererMenu(), "&Renderer");
    menuBar->Append(CreateComponentsMenu(), "&Components");
    menuBar->Append(CreateAboutMenu(), "&About");
    SetMenuBar(menuBar);
}

wxMenu* MainFrame::CreateFileMenu()
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_MENU_FILE_NEW_SCENE, "&New scene", "Create new scene");
    menuFile->Append(ID_MENU_FILE_OPEN_SCENE, "&Open scene", "OpenScene");
    menuFile->Append(ID_MENU_FILE_RELOAD_SCENE, "&Reload scene", "Reload current scene");
    menuFile->Append(ID_MENU_FILE_SAVE_SCENE, "&Save scene\tCtrl-S", "Save scene to known file");
    menuFile->Append(ID_MENU_FILE_SAVEAS_SCENE, "&Save scene as\tCtrl-S", "Save scene to new file");
    menuFile->Append(ID_MENU_FILE_EXIT, "&Exit", "Close editor");
    return menuFile;
}

wxMenu* MainFrame::CreateEditMenu()
{
    wxMenu* menu = new wxMenu;
    menu->Append(ID_MENU_EDIT_CREATE_OBJECT, "&Create new object", "Create empty new object");
    menu->Append(ID_MENU_EDIT_CREATE_TERRAIN, "&Create terrain", "Create gameobject with terrain components");
    menu->Append(ID_MENU_EDIT_MATERIAL_EDITOR, "&Material editor", "Create new default material");
    menu->Append(ID_MENU_EDIT_LOAD_PREFAB, "&Load from prefab", "Create new object");
    menu->Append(ID_MENU_EDIT_CLEAR_SCENE, "&Clear", "Delete all object in scene");
    menu->Append(ID_MENU_EDIT_PREFERENCES, "&Preferences", "Change settings");
    return menu;
}

wxMenu* MainFrame::CreateRendererMenu()
{
    rendererMenu = new wxMenu;
    rendererMenu->Append(ID_MENU_RENDERER_RELOAD_SHADERS, "&Reload shaders", "Reload current shaders");
    rendererMenu->Append(ID_MENU_RENDERER_TAKE_RENDERER_SNAPSHOT, "&Take snapshot", "Create snapshot of renderer state");
    rendererMenu->Append(ID_MENU_RENDERER_SWAP, "&Swap render mode", "Switch between line and fill render mode");
    rendererMenu->Append(ID_MENU_RENDERER_PHYSICS_VISUALIZATION, "&Visualize physics", "Enable/Disable of physics visualization",
                         wxITEM_CHECK);
    rendererMenu->Append(ID_MENU_RENDERER_BOUNDING_BOX_VISUALIZATION, "&Visualize gameObject boundig box",
                         "Enable/Disable of bounding box visualization from engine not from rigidbody physics", wxITEM_CHECK);
    rendererMenu->Append(ID_MENU_RENDERER_NORMAL_VISUALIZATION, "&Visualize normals", "Enable/Disable of normals visualization",
                         wxITEM_CHECK);

    rendererTextureSubMenu = new wxMenu;
    rendererTextureSubMenu->Append(ID_MENU_RENDERER_TEXTURE_AMBIENT, "&Ambient", "Enable/Disable of ambient visualization",
                                   wxITEM_CHECK);
    rendererTextureSubMenu->Append(ID_MENU_RENDERER_TEXTURE_DIFFUSE, "&Diffuse", "Enable/Disable of diffuse visualization",
                                   wxITEM_CHECK);
    rendererTextureSubMenu->Append(ID_MENU_RENDERER_TEXTURE_NORMALS, "&Normals", "Enable/Disable of normals visualization",
                                   wxITEM_CHECK);
    rendererTextureSubMenu->Append(ID_MENU_RENDERER_TEXTURE_SPECULAR, "&Specular", "Enable/Disable of specular visualization",
                                   wxITEM_CHECK);
    rendererTextureSubMenu->Append(ID_MENU_RENDERER_TEXTURE_DISPLACEMENT, "&Displacment",
                                   "Enable/Disable of displacement visualization", wxITEM_CHECK);

    rendererMenu->AppendSubMenu(rendererTextureSubMenu, "&Texture rendering", "Enable/Disable of normals visualization");
    return rendererMenu;
}

void MainFrame::UpdateMainMenuRendererOptionsCheckBoxes()
{
    auto& renderesManager = canvas->GetEngine().GetEngineContext().GetRenderersManager();
    rendererMenu->Check(ID_MENU_RENDERER_PHYSICS_VISUALIZATION,
                        renderesManager.GetDebugRenderer().IsStateEnabled(GameEngine::DebugRenderer::RenderState::Physics));
    rendererMenu->Check(ID_MENU_RENDERER_BOUNDING_BOX_VISUALIZATION,
                        renderesManager.GetDebugRenderer().IsStateEnabled(GameEngine::DebugRenderer::RenderState::BoundingBox));
    rendererMenu->Check(ID_MENU_RENDERER_NORMAL_VISUALIZATION,
                        renderesManager.GetDebugRenderer().IsStateEnabled(GameEngine::DebugRenderer::RenderState::Normals));

    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_AMBIENT, EngineConf.renderer.textures.useAmbient);
    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_DIFFUSE, EngineConf.renderer.textures.useDiffuse);
    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_NORMALS, EngineConf.renderer.textures.useNormal);
    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_SPECULAR, EngineConf.renderer.textures.useSpecular);
    rendererMenu->Check(ID_MENU_RENDERER_TEXTURE_DISPLACEMENT, EngineConf.renderer.textures.useDisplacement);
}

wxMenu* MainFrame::CreateComponentsMenu()
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_MENU_COMPONENTS_REBUILD, "&Rebuild all project components", "Rebuild all project components");
    menuFile->Append(ID_MENU_COMPONENTS_RELOAD, "&Reload all project components", "Reload all project components");

    return menuFile;
}

wxMenu* MainFrame::CreateAboutMenu()
{
    wxMenu* menuAbout = new wxMenu;
    menuAbout->Append(ID_MENU_ABOUT_GL_INFO, "&GLInfo\tCtrl-H", "Print opengl info");
    return menuAbout;
}

void MainFrame::CreateToolBarForEngine()
{
    auto* toolbar = CreateToolBar();

    toolbar->AddTool(ID_TOOL_OPEN, "Open", wxArtProvider::GetBitmap(wxART_FILE_OPEN));
    toolbar->AddTool(ID_TOOL_SAVE, "Save", wxArtProvider::GetBitmap(wxART_FILE_SAVE));
    toolbar->AddTool(ID_TOOL_SAVE_AS, "SaveAs", wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS));
    toolbar->AddTool(ID_TOOL_START, "Start", wxArtProvider::GetBitmap(wxART_GO_FORWARD));
    toolbar->AddTool(ID_TOOL_STOP, "Stop", wxArtProvider::GetBitmap(wxART_CROSS_MARK));

    auto size = toolbar->GetToolBitmapSize();
    toolbar->AddTool(ID_TOOL_ANIMATION_VIEWER, "Animation Viewer",
                     createImage(AnimationViwerIcon_png, AnimationViwerIcon_png_len, size));
    toolbar->AddTool(ID_TOOL_BUILD, "Build components", wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE));
    toolbar->AddTool(ID_TOOL_TERRAIN_PANEL, "Terrain Tools", wxArtProvider::GetBitmap(wxART_HELP_SIDE_PANEL, wxART_TOOLBAR));

    toolbar->SetToolShortHelp(ID_TOOL_OPEN, "Open scene from file");
    toolbar->SetToolShortHelp(ID_TOOL_SAVE, "Save scene");
    toolbar->SetToolShortHelp(ID_TOOL_SAVE_AS, "Save scene to other file");
    toolbar->SetToolShortHelp(ID_TOOL_START, "Start scene");
    toolbar->SetToolShortHelp(ID_TOOL_STOP, "Stop scene if started");
    toolbar->SetToolShortHelp(ID_TOOL_ANIMATION_VIEWER, "Start animation viewer tool");
    toolbar->SetToolShortHelp(ID_TOOL_BUILD, "Build projects componets to create shared libs");
    toolbar->SetToolShortHelp(ID_TOOL_TERRAIN_PANEL, "Show terrain tool panel");
    // Separator zeby odsunac
    toolbar->AddSeparator();

    auto* timeLabel = new wxStaticText(toolbar, wxID_ANY, "Game time: ");
    toolbar->AddControl(timeLabel);

    hourCtrl = new wxSpinCtrl(toolbar, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
    hourCtrl->SetRange(0, 23);
    toolbar->AddControl(hourCtrl);

    minuteCtrl = new wxSpinCtrl(toolbar, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
    minuteCtrl->SetRange(0, 59);
    toolbar->AddControl(minuteCtrl);

    // Suwak godziny (0-24)
    timeSlider = new wxSlider(toolbar, wxID_ANY, 0, 0, 1440, wxDefaultPosition, wxSize(200, -1), wxSL_HORIZONTAL);
    toolbar->AddControl(timeSlider);

    toolbar->Realize();

    timeSlider->Bind(wxEVT_SLIDER,
                     [this](wxCommandEvent& evt)
                     {
                         int minutes      = evt.GetInt();       // 0-1440
                         float normalized = minutes / 1440.0f;  // w [0.0 - 1.0]
                         canvas->GetScene().GetDayNightCycle().SetTime(normalized);

                         // (opcjonalnie: update UI, np. status bar)
                         int hour   = minutes / 60;
                         int minute = minutes % 60;
                         hourCtrl->SetValue(hour);
                         minuteCtrl->SetValue(minute);
                     });

    hourCtrl->Bind(wxEVT_SPINCTRL,
                   [this](wxSpinEvent& evt)
                   {
                       int hour         = evt.GetInt();
                       int minute       = minuteCtrl->GetValue();
                       int totalMinutes = hour * 60 + minute;

                       float normalized = totalMinutes / 1440.0f;
                       canvas->GetScene().GetDayNightCycle().SetTime(normalized);

                       timeSlider->SetValue(totalMinutes);
                   });

    // minuteCtrl -> update slider
    minuteCtrl->Bind(wxEVT_SPINCTRL,
                     [this](wxSpinEvent& evt)
                     {
                         int hour         = hourCtrl->GetValue();
                         int minute       = evt.GetInt();
                         int totalMinutes = hour * 60 + minute;

                         float normalized = totalMinutes / 1440.0f;
                         canvas->GetScene().GetDayNightCycle().SetTime(normalized);

                         timeSlider->SetValue(totalMinutes);
                     });
}

void MainFrame::UpdateTimeOnToolbar()
{
    auto [hour, minute, _] = canvas->GetScene().GetDayNightCycle().GetHourMinuteSecond();
    int totalMinutes       = hour * 60 + minute;

    LOG_DEBUG << "hour: " << hour << " minute: " << minute;
    timeSlider->SetValue(totalMinutes);
    hourCtrl->SetValue(hour);
    minuteCtrl->SetValue(minute);
}

void MainFrame::OnFileSelectChanged(wxTreeEvent& event)
{
}

void MainFrame::OnFileActivated(const wxString& fullpath)
{
    GameEngine::File file{fullpath.ToStdString()};
    LOG_DEBUG << file;

    if (is3dModelFile(file))
    {
        auto parentGameObject = GetSelectedGameObject();
        if (auto id = canvas->AddGameObject(file, parentGameObject))
        {
            gameObjectsView->SelectItemWhenGameObjectBecomeAvaiable(*id);
        }
    }
    else if (isPrefab(file))
    {
        LoadPrefab(file.GetAbsolutePath().string());
    }
}

void MainFrame::OnObjectTreeSelChange(wxTreeEvent& event)
{
    LOG_DEBUG << "";
    wxTreeItemId itemId = event.GetItem();
    if (not itemId.IsOk())
        return;

    RemoveAllComponentPanels();
    if (itemId == gameObjectsView->GetRootItem())
    {
        worldTransformPanel->lock();
        localTransformPanel->lock();
        return;
    }

    auto go = GetSelectedGameObject();
    if (go)
    {
        LOG_DEBUG << "go";
        LOG_DEBUG << "id =" << go->GetId();
        UpdateGameObjectIdOnTransfromLabel(go->GetId());
        worldTransformPanel->set(go->GetWorldTransform());
        localTransformPanel->set(go->GetLocalTransform());

        if (not isGameObjectPrefab(*go->GetParent()))
        {
            worldTransformPanel->unlock();
            localTransformPanel->unlock();
        }
        else if (not isGameObjectPrefab(*go->GetParent()))
        {
            worldTransformPanel->unlock();
            localTransformPanel->unlock();
        }
        else
        {
            worldTransformPanel->lock();
            localTransformPanel->lock();
        }

        if (not transfromSubController)
        {
            transfromSubController.emplace(*canvas, worldTransformPanel, localTransformPanel, go->GetId());
        }
        else
        {
            transfromSubController->ChangeGameObject(go->GetId());
        }
        AddGameObjectComponentsToView(*go);
    }
}

void MainFrame::OnObjectTreeActivated(wxTreeEvent& event)
{
    if (auto maybeGameObjectId = gameObjectsView->Get(event.GetItem().GetID()))
    {
        auto& scene = canvas->GetScene();
        if (auto gameObject = scene.GetGameObject(*maybeGameObjectId))
        {
            scene.GetCamera().SetPosition(gameObject->GetWorldTransform().GetPosition() +
                                          (gameObject->GetWorldTransform().GetScale() + vec3(1.f)));
            scene.GetCamera().LookAt(gameObject->GetWorldTransform().GetPosition());
        }
    }
}

void MainFrame::OnBeginLabelEdit(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();
    if (item == gameObjectsView->GetRootItem())
    {
        event.Veto();
    }
}

void MainFrame::OnEndLabelEdit(wxTreeEvent& event)
{
    if (!event.IsEditCancelled())
    {
        wxString newLabel = event.GetLabel();
        if (newLabel.IsEmpty())
        {
            wxMessageBox("Nazwa nie moze byc pusta");
            event.Veto();  // Anuluj zmiane
        }
        else
        {
            auto go = GetSelectedGameObject();
            if (go)
            {
                go->SetName(newLabel.ToStdString());
            }
        }
    }
}

void MainFrame::OnRename(wxCommandEvent& event)
{
    auto item = gameObjectsView->GetSelection();
    if (item.IsOk())
    {
        gameObjectsView->EditLabel(item);
    }
}

void MainFrame::OnAddObject(wxCommandEvent& event)
{
    MenuEditCreateObject(event);
}

void MainFrame::OnDeleteObject(wxCommandEvent& event)
{
    auto sel = gameObjectsView->GetSelection();
    if (sel.IsOk() and sel.GetID() != gameObjectsView->GetRootItem())
    {
        auto gameObjectId = gameObjectsView->Get(sel);
        if (gameObjectId)
        {
            int answer = wxMessageBox("Delete game object " + gameObjectsView->GetItemText(sel) + "?", "Confirmation",
                                      wxYES_NO | wxICON_QUESTION);

            if (answer != wxYES)
            {
                return;
            }

            LOG_DEBUG << "Remove gameObject";
            canvas->GetScene().RemoveGameObject(*gameObjectId);
            RemoveAllComponentPanels();
        }
        else
        {
            wxLogMessage("Game object not found in scene");
        }
    }
}

void MainFrame::OnUnmarkPrefab(wxCommandEvent&)
{
    auto sel = gameObjectsView->GetSelection();

    if (sel.IsOk())
    {
        if (auto maybeGo = GetGameObject(sel))
        {
            if (isGameObjectPrefab(*maybeGo))
            {
                if (not maybeGo->GetChildren().empty())
                {
                    if (auto parent = maybeGo->GetParent())
                    {
                        auto& rootGameObjectOfPrefab = maybeGo->GetChildren().front();

                        // WxWidgets swap
                        auto parentItemId     = gameObjectsView->Get(parent->GetId());
                        auto currentItemId    = gameObjectsView->Get(maybeGo->GetId());
                        auto prefabRootItemId = gameObjectsView->Get(rootGameObjectOfPrefab->GetId());

                        if (currentItemId)
                            parentItemId = gameObjectsView->GetItemParent(*currentItemId);

                        if (parentItemId and prefabRootItemId)
                        {
                            gameObjectsView->MoveTreeNode(*prefabRootItemId, *parentItemId);

                            if (currentItemId and not gameObjectsView->ItemHasChildren(*currentItemId))
                                gameObjectsView->Delete(*currentItemId);
                        }

                        // Engine swap
                        ChangeGameObjectParent(*rootGameObjectOfPrefab, *parent);
                        canvas->GetScene().RemoveGameObject(maybeGo->GetId());
                    }
                }
            }
        }
    }
}

void MainFrame::OnMakePrefab(wxCommandEvent&)
{
    if (auto maybeGo = GetSelectedGameObject())
    {
        wxFileDialog fileDialog(this, "Choose prefab file", Utils::GetAbsolutePath(EngineConf.files.getDataPath()).string(),
                                maybeGo->GetName() + ".prefab", "Pliki prefabow (*.prefab)|*.prefab",
                                wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        if (fileDialog.ShowModal() == wxID_CANCEL)
            return;

        wxString path = fileDialog.GetPath();
        GameEngine::createAndSavePrefab(GameEngine::File{path.c_str()}, *maybeGo);
    }
}

void MainFrame::CloneGameObject(wxCommandEvent& event)
{
    if (auto maybeGo = GetSelectedGameObject())
    {
        GameEngine::cloneGameObject(canvas->GetScene(), *maybeGo);
    }
}

void MainFrame::OnPageChanged(wxBookCtrlEvent& event)
{
    int selection = event.GetSelection();
    if (transfromSubController)
    {
        LOG_DEBUG << "selection = " << selection;
        transfromSubController->ChangeState(static_cast<TransfromSubController::State>(selection));
    }

    event.Skip();
}

void MainFrame::ChangeGameObjectParent(GameEngine::GameObject& object, GameEngine::GameObject& newParent)
{
    canvas->GetScene().ChangeParent(object, newParent);
}

void MainFrame::OnToolStart(wxCommandEvent& event)
{
    GameEngine::File sceneFile{"editorTmpSceneToRun.xml"};
    auto orginalFile = canvas->GetScene().GetFile();
    GameEngine::saveSceneToFile(canvas->GetScene(), sceneFile);
    canvas->GetScene().GetFile() = orginalFile;

    std::string cmd = "\"" + wxStandardPaths::Get().GetExecutablePath().ToStdString() + "\" --scene \"" +
                      sceneFile.GetAbsolutePath().string() + "\" " + "--projectPath " +
                      ProjectManager::GetInstance().GetProjectPath().string();

    long pid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_NOHIDE | wxEXEC_NODISABLE);
    if (pid == 0)
    {
        wxLogError("Run scene error!");
        return;
    }
    startedGameProceesId = pid;
}

void MainFrame::OnToolStop(wxCommandEvent&)
{
    if (startedGameProceesId)
    {
        terminateProcessByPID(*startedGameProceesId);
        startedGameProceesId.reset();
    }
}

void MainFrame::OnToolAnimationViewer(wxCommandEvent&)
{
    runAnimationViewer();
}

void MainFrame::OnBuildCmponents(wxCommandEvent&)
{
    LOG_DEBUG << "Build components";
    BuildComponentLogFrame::ReloadComponents reloadComponents = [this]()
    {
        canvas->GetEngine().getExternalComponentsReader().ReloadAll();
        RemoveAllComponentPanels();

        if (auto gameObject = GetSelectedGameObject())
        {
            AddGameObjectComponentsToView(*gameObject);
        }
    };

    auto* logFrame = new BuildComponentLogFrame(this, projectPanel, reloadComponents);
    logFrame->Build();
}

void MainFrame::OnToggleTerrainPanel(wxCommandEvent& event)
{
    if (!rightSplitter)
        return;

    auto onTerrainPanelClose = [this]()
    {
        rightSplitter->Unsplit(terrainPanel);
        terrainPanelVisible = false;
        canvas->EnablePicker();
    };

    if (not terrainPanel)
    {
        int width    = 300;
        terrainPanel = new TerrainToolPanel(rightSplitter, canvas->GetScene(), width);
        terrainPanel->Bind(wxEVT_BUTTON,
                           [onTerrainPanelClose](wxCommandEvent& event)
                           {
                               if (event.GetId() == wxID_CLOSE)
                               {
                                   onTerrainPanelClose();
                               }

                               event.Skip();
                           });
    }

    if (terrainPanelVisible)
    {
        onTerrainPanelClose();
    }
    else
    {
        rightSplitter->SplitVertically(canvas, terrainPanel, GetSize().x - 300);
        rightSplitter->SetSashGravity(1.0);
        rightSplitter->SetMinimumPaneSize(300);
        terrainPanelVisible = true;
        canvas->DisablePicker();
    }

    rightSplitter->Layout();

    return;
}

bool MainFrame::SaveSceneAs()
{
    wxFileDialog fileDialog(this, "Wybierz plik", ProjectManager::GetInstance().GetScenesDir().string(), "",
                            "Pliki sceny (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*", wxFD_SAVE);

    if (fileDialog.ShowModal() == wxID_CANCEL)
        return false;

    wxString path = fileDialog.GetPath();
    SaveSceneAs(path.ToStdString());
    return true;
}

void MainFrame::SaveSceneAs(const std::string& path)
{
    GameEngine::File file{path.c_str()};
    canvas->GetScene().ChangeName(file.GetBaseName());
    SetTitle("Active scene : " + canvas->GetScene().GetName());
    GameEngine::saveSceneToFile(canvas->GetScene(), GameEngine::File{file});
}

void MainFrame::UpdateGameObjectIdOnTransfromLabel(std::optional<IdType> maybeId)
{
    std::string id = "-";
    if (maybeId)
    {
        id = std::to_string(*maybeId);
    }
    transformsCollapsible->SetLabelText("Transform, gameObject id: " + id);
}

TransfromSubController::TransfromSubController(GLCanvas& canvas, TransformPanel* worldTranformPanel,
                                               TransformPanel* localTransformPanel, GameObjectId goId)
    : canvas{canvas}
    , transformPanels{worldTranformPanel, localTransformPanel}
    , gameObjectId{goId}
    , state{State::world}
{
    LOG_DEBUG << "SubscribeCurrent";
    SubscribeCurrent();
    for (auto& panel : transformPanels)
    {
        panel->set(
            [&](const auto& label, const auto& v)
            {
                auto gameObject = canvas.GetScene().GetGameObject(gameObjectId);
                if (gameObject)
                {
                    if (label == LABEL_POSITION)
                    {
                        if (state == State::world)
                        {
                            gameObject->SetWorldPosition(v);
                        }
                        else
                        {
                            gameObject->SetLocalPosition(v);
                        }
                    }
                    else if (label == LABEL_ROTATION)
                    {
                        if (state == State::world)
                        {
                            gameObject->SetWorldRotation(Rotation{DegreesVec3(v)});
                        }
                        else
                        {
                            gameObject->SetLocalRotation(Rotation{DegreesVec3(v)});
                        }
                    }
                    else if (label == LABEL_SCALE)
                    {
                        if (state == State::world)
                        {
                            gameObject->SetWorldScale(v);
                        }
                        else
                        {
                            gameObject->SetLocalScale(v);
                        }
                    }
                }
            });
    }
}

TransfromSubController::~TransfromSubController()
{
    UnsubscribeCurrent();
}

void TransfromSubController::ChangeGameObject(GameObjectId goId)
{
    if (gameObjectId != goId)
    {
        LOG_DEBUG << "ChangeGameObject " << goId;
        UnsubscribeCurrent();
        gameObjectId = goId;
        SubscribeCurrent();
    }
}

void TransfromSubController::ChangeState(State s)
{
    if (state != s)
    {
        LOG_DEBUG << "State " << magic_enum::enum_name(s);
        UnsubscribeCurrent();
        state = s;
        SubscribeCurrent();
    }
}

void TransfromSubController::SubscribeCurrent()
{
    auto go = canvas.GetScene().GetGameObject(gameObjectId);
    if (go)
    {
        auto updatePanel = [&](const auto& transform) { transformPanels[state]->set(transform); };

        if (state == State::world)
        {
            LOG_DEBUG << "Sub world, " << go->GetName();
            subId = go->SubscribeOnWorldTransfomChange(updatePanel);
            transformPanels[state]->set(go->GetWorldTransform());
        }
        else
        {
            LOG_DEBUG << "Sub local, " << go->GetName();
            subId = go->SubscribeOnLocalTransfomChange(updatePanel);
            transformPanels[state]->set(go->GetLocalTransform());
        }
    }
}

void TransfromSubController::UnsubscribeCurrent()
{
    if (subId)
    {
        auto go = canvas.GetScene().GetGameObject(gameObjectId);
        if (go)
        {
            if (state == State::world)
            {
                LOG_DEBUG << "Unsub worl, " << go->GetName();
                go->UnsubscribeOnWorldTransfromChange(*subId);
            }
            else
            {
                LOG_DEBUG << "Uub local, " << go->GetName();
                go->UnsubscribeOnLocalTransfromChange(*subId);
            }
        }
    }

    subId.reset();
}
