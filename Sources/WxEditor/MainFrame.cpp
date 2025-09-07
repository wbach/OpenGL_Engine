#include "MainFrame.h"

#include <GameEngine/DebugTools/Painter/TerrainHeightPainter.h>
#include <GameEngine/DebugTools/Painter/TerrainTexturePainter.h>
#include <GameEngine/Scene/SceneEvents.h>
#include <GameEngine/Scene/SceneReader.h>
#include <GameEngine/Scene/SceneUtils.h>
#include <wx/artprov.h>
#include <wx/defs.h>
#include <wx/filedlg.h>
#include <wx/splitter.h>
#include <wx/statbmp.h>
#include <wx/stdpaths.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>
#include <iostream>
#include <string>

#include "ComponentPanel.h"
#include "ComponentPickerPopup.h"
#include "ControlsIds.h"
#include "Engine/Configuration.h"
#include "GLCanvas.h"
#include "Logger/Log.h"
#include "OptionsFrame.h"
#include "ProjectManager.h"
#include "ProjectPanel.h"
#include "Theme.h"
#include "TransformPanel.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>

#include <csignal>
#endif

namespace
{
bool terminateProcessByPID(long pid)
{
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(pid));
    if (!hProcess)
    {
        std::cerr << "Nie można otworzyć procesu, PID=" << pid << "\n";
        return false;
    }

    if (!TerminateProcess(hProcess, 0))
    {
        std::cerr << "Nie udało się zakończyć procesu, PID=" << pid << "\n";
        CloseHandle(hProcess);
        return false;
    }

    CloseHandle(hProcess);
    return true;

#else
    if (kill(static_cast<pid_t>(pid), SIGTERM) != 0)
    {
        perror("Nie udało się zakończyć procesu");
        return false;
    }
    return true;
#endif
}

bool isGameObjectPrefab(const GameEngine::GameObject& go)
{
    std::function<bool(const GameEngine::GameObject&)> checkPrefab;
    checkPrefab = [&checkPrefab](const GameEngine::GameObject& obj) -> bool
    {
        if (dynamic_cast<const GameEngine::Prefab*>(&obj) != nullptr)
            return true;

        if (obj.GetParent() != nullptr)
            return checkPrefab(*obj.GetParent());

        return false;
    };

    return checkPrefab(go);
}
}  // namespace

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_MENU(ID_MENU_FILE_NEW_PROJECT, MainFrame::MenuFileNewProject)
    EVT_MENU(ID_MENU_FILE_OPEN_PROJECT, MainFrame::MenuFileOpenProject)
    EVT_MENU(ID_MENU_FILE_OPEN_SCENE, MainFrame::MenuFileOpenScene)
    EVT_MENU(ID_MENU_FILE_SAVE_SCENE, MainFrame::MenuFileSaveScene)
    EVT_MENU(ID_MENU_FILE_SAVEAS_SCENE, MainFrame::MenuFileSaveSceneAs)
    EVT_MENU(ID_MENU_FILE_EXIT, MainFrame::MenuFileExit)
    EVT_MENU(ID_MENU_EDIT_CREATE_OBJECT, MainFrame::MenuEditCreateObject)
    EVT_MENU(ID_MENU_EDIT_CREATE_TERRAIN, MainFrame::MenuEditCreateTerrain)
    EVT_MENU(ID_MENU_EDIT_TERRAIN_HEIGHT_PAINTER, MainFrame::MenuEditTerrainHeightPainter)
    EVT_MENU(ID_MENU_EDIT_TERRAIN_TEXTURE_PAINTER, MainFrame::MenuEditTerrainTexturePainter)
    EVT_MENU(ID_MENU_EDIT_LOAD_PREFAB, MainFrame::MenuEditLoadPrefab)
    EVT_MENU(ID_MENU_EDIT_CLEAR_SCENE, MainFrame::MenuEditClearScene)
    EVT_MENU(ID_MENU_EDIT_PREFERENCES, MainFrame::MenuEditPreferences)
    EVT_MENU(ID_MENU_RENDERER_RELOAD_SHADERS, MainFrame::MenuRendererReloadShaders)
    EVT_MENU(ID_MENU_RENDERER_TAKE_RENDERER_SNAPSHOT, MainFrame::MenuRendererTakeSnapshot)
    EVT_MENU(ID_MENU_RENDERER_SWAP, MainFrame::MenuRendererSwap)
    EVT_MENU(ID_MENU_RENDERER_PHYSICS_VISUALIZATION, MainFrame::MenuRendererPhysicsVisualization)
    EVT_MENU(ID_MENU_RENDERER_NORMAL_VISUALIZATION, MainFrame::MenuRendererNormalsVisualization)
    EVT_MENU(ID_MENU_RENDERER_TEXTURE_DIFFUSE, MainFrame::MenuRendererTextureDiffuse)
    EVT_MENU(ID_MENU_RENDERER_TEXTURE_NORMALS, MainFrame::MenuRendererTextureNormals)
    EVT_MENU(ID_MENU_RENDERER_TEXTURE_SPECULAR, MainFrame::MenuRendererTextureSpecular)
    EVT_MENU(ID_MENU_EDIT_CLEAR_SCENE, MainFrame::MenuRendererTextureDisplacement)
    EVT_MENU(ID_MENU_ABOUT_GL_INFO, MainFrame::OnGLVersion)
    EVT_TREE_SEL_CHANGED(ID_OBJECT_TREE, MainFrame::OnObjectTreeSelChange)
    EVT_TREE_ITEM_ACTIVATED(ID_OBJECT_TREE, MainFrame::OnObjectTreeActivated)
//    EVT_TREE_BEGIN_DRAG(ID_OBJECT_TREE, MainFrame::OnObjectDrag)
//    EVT_TREE_END_DRAG(ID_OBJECT_TREE, MainFrame::OnObjectEndDrag)
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

wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
// clang-format on
{
    // Init();
}

void MainFrame::Init()
{
    wxInitAllImageHandlers();

    // Główny splitter: lewy/prawy
    wxSplitterWindow* mainSplitter = new wxSplitterWindow(this, wxID_ANY);

    // Lewy splitter: góra/dół
    wxSplitterWindow* leftSplitter = new wxSplitterWindow(mainSplitter, wxID_ANY);

    // Góra lewego splittera: tree + canvas
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
        SetStatusText(EngineConf.files.data);
    };
    auto selectItemInGameObjectTree = [this](uint32 gameObjectId, bool select)
    {
        if (auto wxItemId = gameObjectsView->Get(gameObjectId))
        {
            gameObjectsView->SelectItem(*wxItemId, select);
            UpdateGameObjectIdOnTransfromLabel(gameObjectId);
        }
    };
    canvas    = new GLCanvas(topSplitter, onStartupDone, selectItemInGameObjectTree);
    auto size = GetSize();
    // Split pionowy: tree + canvas
    topSplitter->SplitVertically(gameObjectsView->GetWxTreeCtrl(), canvas, size.x / 8);

    // === Dół: ProjectPanel ===
    auto fileSelectedCallback = [this](const wxString& str) { OnFileActivated(str); };
    projectPanel =
        new ProjectPanel(leftSplitter, ProjectManager::GetInstance().GetProjectPath(), fileSelectedCallback);

    // Lewy splitter: góra (tree+canvas), dół (projectPanel)
    leftSplitter->SplitHorizontally(topSplitter, projectPanel, size.y * 3 / 5);

    // === Prawa strona: gameObjectPanels ===
    gameObjectPanels = new wxScrolledWindow(mainSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    gameObjectPanels->SetScrollRate(5, 5);  // ustaw scrollowanie

    gameObjectPanelsSizer = new wxBoxSizer(wxVERTICAL);
    gameObjectPanels->SetSizer(gameObjectPanelsSizer);

    // Tworzymy collapsible, który będzie "kontenerem" dla notebooka
    transformsCollapsible = new wxCollapsiblePane(gameObjectPanels, wxID_ANY, "Transform");
    UpdateGameObjectIdOnTransfromLabel();

    gameObjectPanelsSizer->Add(transformsCollapsible, 0, wxEXPAND | wxALL, 0);

    // Teraz do collapsible dajemy notebook
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

    // Powiązanie zdarzenia z ID_SAVE
    Bind(wxEVT_MENU, &MainFrame::MenuFileSaveScene, this, ID_SAVE);
    SaveOsTheme(*this);
    // ApplyTheme(*this);

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
    canvas->GetScene().ClearGameObjects();
    gameObjectsView->DeleteAllItems();
    canvas->ResetDragObject();
    transfromSubController.reset();
    RemoveAllComponentPanels();
}

void MainFrame::OnClose(wxCloseEvent& event)
{
    DEBUG_LOG("OnClose");
    isRunning = false;
    if (loadSceneThread.joinable())
    {
        loadSceneThread.join();
    }
    event.Skip();
}

void MainFrame::MenuFileNewProject(wxCommandEvent&)
{
    // Step 1: Ask for project name
    wxTextEntryDialog nameDialog(this, "Enter project name:", "New Project");

    if (nameDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString projectName = nameDialog.GetValue();
    if (projectName.IsEmpty())
    {
        wxMessageBox("Project name cannot be empty.", "Error", wxICON_ERROR | wxOK);
        return;
    }

    // Step 2: Choose directory
    wxDirDialog openDirDialog(this, "Choose a directory for the new project",
                              Utils::GetAbsolutePath(EngineConf.files.data + "/Scenes"),
                              wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | wxDD_NEW_DIR_BUTTON);

    if (openDirDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString selectedDir = openDirDialog.GetPath();

    // Step 3: Create project subfolder
    wxString projectDir = selectedDir + "/" + projectName;

    // Step 4: Update ProjectManager
    auto& pm = ProjectManager::GetInstance();
    pm.SetProjectPath(projectDir.ToStdString());
    pm.SetProjectName(projectName.ToStdString());

    auto defualtMainScene = pm.GetScenesDir() + "/main.xml";
    SaveSceneAs(defualtMainScene);
    GameEngine::CreateDefaultFile(pm.GetConfigFile());
    GameEngine::createScenesFile(pm.GetScenesFactoryFile());

    wxMessageBox("Project created at:\n" + projectDir, "Success", wxICON_INFORMATION | wxOK);
}

void MainFrame::MenuFileOpenProject(wxCommandEvent&)
{
    wxMessageBox("NotImplemented yet", "Confirmation", wxOK | wxICON_QUESTION);
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

    wxFileDialog openFileDialog(this, "Wybierz plik", ProjectManager::GetInstance().GetScenesDir(), "",
                                "Pliki sceny (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    ClearScene();

    wxString path = openFileDialog.GetPath();
    GameEngine::File file{std::string{path.c_str()}};
    canvas->OpenScene(file,
                      [&]()
                      {
                          if (isRunning)
                          {
                              AddChilds(canvas->GetRootObject(), gameObjectsView->GetRootItem());
                              UpdateObjectCount();
                              SetStatusText("Welcome to game editor!");
                              SetTitle("Active scene : " + canvas->GetScene().GetName());
                          }
                      });
    SetStatusText("Loading file " + file.GetBaseName());
}

void MainFrame::MenuFileReloadScene(wxCommandEvent&)
{
    canvas->GetScene().addSceneEvent(GameEngine::SceneEventType::RELOAD_SCENE);
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
    auto parentGameObjectId = canvas->GetScene().GetRootGameObject().GetId();
    if (auto parentGameObject = GetSelectedGameObject())
    {
        parentGameObjectId = parentGameObject->GetId();
    }
    auto gameObject = AddGameObject("NewGameObject", parentGameObjectId);
    auto itemId     = gameObjectsView->AppendItem(gameObjectsView->GetSelection(), gameObject->GetName(), gameObject->GetId());
    gameObjectsView->SelectItem(itemId);
    UpdateGameObjectIdOnTransfromLabel(gameObject->GetId());
    UpdateObjectCount();
}

GameEngine::GameObject* MainFrame::AddGameObject(const std::string& name, IdType parentId)
{
    auto gameObject       = canvas->GetScene().CreateGameObject(name);
    auto result           = gameObject.get();
    auto parentGameObject = canvas->GetScene().GetGameObject(parentId);

    if (parentGameObject)
    {
        auto go            = gameObject.get();
        auto worldPosition = gameObject->GetWorldTransform().GetPosition();
        auto worldRotation = gameObject->GetWorldTransform().GetRotation();
        auto worldScale    = gameObject->GetWorldTransform().GetScale();
        DEBUG_LOG("NewGameObj add");
        parentGameObject->AddChild(std::move(gameObject));
        go->SetWorldPosition(worldPosition);
        go->SetWorldRotation(worldRotation);
        go->SetWorldScale(worldScale);
    }
    else
    {
        DEBUG_LOG("NewGameObj add");
        canvas->GetScene().AddGameObject(std::move(gameObject));
    }

    return result;
}

void MainFrame::UpdateObjectCount()
{
    auto objectCount = gameObjectsView->GetChildrenCount();
    gameObjectsView->SetItemText(gameObjectsView->GetRootItem(), "Scene (Objects: " + std::to_string(objectCount) + ")");
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
}

GameEngine::Painter::EntryParamters MainFrame::GetPainterEntryParameters()
{
    auto& engineContext = canvas->GetEngine().GetEngineContext();
    auto& scene         = canvas->GetScene();
    return GameEngine::Painter::EntryParamters{engineContext.GetInputManager(), scene.GetCamera(),
                                               engineContext.GetRenderersManager().GetProjection(),
                                               engineContext.GetDisplayManager().GetWindowSize(), scene.getComponentController()};
}

void MainFrame::AddGameObjectComponentsToView(GameEngine::GameObject& gameObject)
{
    for (auto& component : gameObject.GetComponents())
    {
        auto* compPanel = new ComponentPanel(this, gameObjectPanels, canvas->GetEngine().getExternalComponentsReader(),
                                             canvas->GetScene().getComponentController(), gameObject);
        compPanel->AddComponent(*component);
        if (isGameObjectPrefab(gameObject))
        {
            compPanel->Lock();
        }
        gameObjectPanelsSizer->Add(compPanel, 0, wxEXPAND | wxALL, 0);
    }

    if (addComponentButton)
    {
        addComponentButton->Destroy();
    }

    addComponentButton = new wxButton(gameObjectPanels, wxID_ANY, "Add component");
    gameObjectPanelsSizer->Add(addComponentButton, 0, wxEXPAND | wxALL, 0);

    auto action = [&](wxCommandEvent&)
    {
        auto popup =
            new ComponentPickerPopup(gameObjectPanels, canvas->GetScene().getComponentController(), gameObject,
                                     [this, &gameObject](auto& component)
                                     {
                                         ComponentPanel* compPanel = new ComponentPanel(
                                             this, gameObjectPanels, canvas->GetEngine().getExternalComponentsReader(),
                                             canvas->GetScene().getComponentController(), gameObject);
                                         compPanel->AddComponent(component, false);
                                         if (isGameObjectPrefab(gameObject))
                                         {
                                             compPanel->Lock();
                                         }

                                         this->CallAfter(
                                             [this, compPanel]()
                                             {
                                                 int btnIndex = gameObjectPanelsSizer->GetItemCount() - 1;
                                                 if (btnIndex < 0)
                                                     btnIndex = 0;  // zabezpieczenie, gdyby przycisku jeszcze nie było
                                                 gameObjectPanelsSizer->Insert(btnIndex, compPanel, 0, wxEXPAND | wxALL, 0);

                                                 gameObjectPanelsSizer->Layout();
                                                 gameObjectPanels->FitInside();
                                                 gameObjectPanels->Refresh();
                                             });
                                     });

        // Pobieramy pozycję przycisku w globalnych współrzędnych
        wxPoint pos = addComponentButton->GetScreenPosition();

        // Ustawiamy szerokość popupu na szerokość przycisku
        int buttonWidth = addComponentButton->GetSize().GetWidth();
        wxSize popupSize(buttonWidth, 2 * popup->GetSize().GetHeight());
        popup->SetSize(popupSize);

        // Pozycjonujemy i pokazujemy popup
        popup->Position(pos, wxSize(0, 0));
        popup->Popup();
    };
    addComponentButton->Bind(wxEVT_BUTTON, action);

    // Odświeżenie layoutu po dodaniu przycisku
    gameObjectPanelsSizer->Layout();
    gameObjectPanels->FitInside();
    gameObjectPanels->Refresh();
}

void MainFrame::MenuEditTerrainHeightPainter(wxCommandEvent&)
{
    terrainPainter_ = std::make_unique<GameEngine::TerrainHeightPainter>(GetPainterEntryParameters());
}

void MainFrame::MenuEditTerrainTexturePainter(wxCommandEvent&)
{
    terrainPainter_ = std::make_unique<GameEngine::TerrainTexturePainter>(GetPainterEntryParameters(), Color(255, 0, 0));
}

void MainFrame::MenuEditLoadPrefab(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, "Wybierz plik", Utils::GetAbsolutePath(EngineConf.files.data), "",
                                "Pliki prefab (*.prefab)|*.prefab|Pliki prefab (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    auto go = GameEngine::SceneReader::loadPrefab(canvas->GetScene(), openFileDialog.GetPath().ToStdString());
    if (go)
    {
        auto prefabItemId = gameObjectsView->AppendItemToSelection(go->GetName() + " (prefab)", go->GetId());
        gameObjectsView->DisableItem(prefabItemId);

        // TO DO remove duplicate
        std::function<void(wxTreeItemId, GameEngine::GameObject&)> addChildToWidgets;
        addChildToWidgets = [&](wxTreeItemId wxId, GameEngine::GameObject& go)
        {
            const auto& children = go.GetChildren();
            for (const auto& child : children)
            {
                auto childItemId = gameObjectsView->AppendItem(wxId, child->GetName(), child->GetId());
                gameObjectsView->DisableItem(childItemId);
                addChildToWidgets(childItemId, *child);
            }
        };
        addChildToWidgets(prefabItemId, *go);
        UpdateObjectCount();
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
    SetDeubgRendererState(GameEngine::DebugRenderer::RenderState::Physics);
}

void MainFrame::SetDeubgRendererState(GameEngine::DebugRenderer::RenderState state)
{
    auto& renderesManager = canvas->GetEngine().GetEngineContext().GetRenderersManager();
    bool set              = renderesManager.GetDebugRenderer().IsStateEnabled(state);
    set                   = !set;

    auto& debugRenderer = renderesManager.GetDebugRenderer();
    set ? debugRenderer.AddState(state) : debugRenderer.RemoveState(state);
}

void MainFrame::MenuRendererNormalsVisualization(wxCommandEvent&)
{
    SetDeubgRendererState(GameEngine::DebugRenderer::RenderState::Normals);
}

void MainFrame::MenuRendererTextureDiffuse(wxCommandEvent&)
{
    auto& textConf      = EngineConf.renderer.textures;
    textConf.useDiffuse = !textConf.useDiffuse;

    canvas->GetScene().GetResourceManager().GetGpuResourceLoader().AddFunctionToCall(
        [&]() { canvas->GetEngine().GetEngineContext().GetRenderersManager().UpdatePerAppBuffer(); });
}

void MainFrame::MenuRendererTextureNormals(wxCommandEvent&)
{
    auto& textConf     = EngineConf.renderer.textures;
    textConf.useNormal = !textConf.useNormal;

    canvas->GetScene().GetResourceManager().GetGpuResourceLoader().AddFunctionToCall(
        [&]() { canvas->GetEngine().GetEngineContext().GetRenderersManager().UpdatePerAppBuffer(); });
}

void MainFrame::MenuRendererTextureSpecular(wxCommandEvent&)
{
    auto& textConf       = EngineConf.renderer.textures;
    textConf.useSpecular = !textConf.useSpecular;

    canvas->GetScene().GetResourceManager().GetGpuResourceLoader().AddFunctionToCall(
        [&]() { canvas->GetEngine().GetEngineContext().GetRenderersManager().UpdatePerAppBuffer(); });
}

void MainFrame::MenuRendererTextureDisplacement(wxCommandEvent&)
{
    auto& textConf           = EngineConf.renderer.textures;
    textConf.useDisplacement = !textConf.useDisplacement;

    canvas->GetScene().GetResourceManager().GetGpuResourceLoader().AddFunctionToCall(
        [&]() { canvas->GetEngine().GetEngineContext().GetRenderersManager().UpdatePerAppBuffer(); });
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
    menuBar->Append(CreateAboutMenu(), "&About");
    SetMenuBar(menuBar);
}

wxMenu* MainFrame::CreateFileMenu()
{
    wxMenu* menuFile = new wxMenu;
    // menuFile->Append(ID_MENU_FILE_NEW_PROJECT, "&New project", "Create new project");
    // menuFile->Append(ID_MENU_FILE_OPEN_PROJECT, "&Open project", "Open existing project");
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
    menu->Append(ID_MENU_EDIT_CREATE_OBJECT, "&Create new object\tCtrl-A", "Create empty new object");
    menu->Append(ID_MENU_EDIT_CREATE_TERRAIN, "&Create terrain\tCtrl-A", "Create gameobject with terrain components");
    menu->Append(ID_MENU_EDIT_TERRAIN_HEIGHT_PAINTER, "&Terrain height painter \tCtrl-A", "Enable height painter tool");
    menu->Append(ID_MENU_EDIT_TERRAIN_TEXTURE_PAINTER, "&Terrain texture painter \tCtrl-A",
                 "Enable terrain texture painter tool");
    menu->Append(ID_MENU_EDIT_LOAD_PREFAB, "&Load from prefab\tCtrl-A", "Create new object");
    menu->Append(ID_MENU_EDIT_CLEAR_SCENE, "&Clear\tCtrl-A", "Delete all object in scene");
    menu->Append(ID_MENU_EDIT_PREFERENCES, "&Preferences\tCtrl-A", "Change settings");
    return menu;
}

wxMenu* MainFrame::CreateRendererMenu()
{
    wxMenu* menu = new wxMenu;
    menu->Append(ID_MENU_RENDERER_RELOAD_SHADERS, "&Reload shaders\tCtrl-A", "Reload current shaders");
    menu->Append(ID_MENU_RENDERER_TAKE_RENDERER_SNAPSHOT, "&Take snapshot\tCtrl-A", "Create snapshot of renderer state");
    menu->Append(ID_MENU_RENDERER_SWAP, "&Swap render mode\tCtrl-A", "Switch between line and fill render mode");
    menu->Append(ID_MENU_RENDERER_PHYSICS_VISUALIZATION, "&Visualize physics\tCtrl-A", "Enable/Disable of physics visualization");
    menu->Append(ID_MENU_RENDERER_NORMAL_VISUALIZATION, "&Visualize normals\tCtrl-A", "Enable/Disable of normals visualization");

    wxMenu* texturesMenu = new wxMenu;
    texturesMenu->Append(ID_MENU_RENDERER_TEXTURE_DIFFUSE, "&Diffuse\tCtrl-A", "Enable/Disable of normals visualization");
    texturesMenu->Append(ID_MENU_RENDERER_TEXTURE_NORMALS, "&Normals\tCtrl-A", "Enable/Disable of normals visualization");
    texturesMenu->Append(ID_MENU_RENDERER_TEXTURE_SPECULAR, "&Specular\tCtrl-A", "Enable/Disable of normals visualization");
    texturesMenu->Append(ID_MENU_RENDERER_TEXTURE_DISPLACEMENT, "&Displacment\tCtrl-A",
                         "Enable/Disable of normals visualization");

    menu->AppendSubMenu(texturesMenu, "&Texture rendering\tCtrl-A", "Enable/Disable of normals visualization");
    return menu;
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
    toolbar->AddTool(ID_TOOL_ANIMATION_VIEWER, "Animation Viewer",
                     wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_TOOLBAR));
    toolbar->AddTool(ID_TOOL_BUILD, "Build components", wxArtProvider::GetBitmap(wxART_FLOPPY));

    toolbar->SetToolShortHelp(ID_TOOL_OPEN, "Open scene from file");
    toolbar->SetToolShortHelp(ID_TOOL_SAVE, "Save scene");
    toolbar->SetToolShortHelp(ID_TOOL_SAVE_AS, "Save scene to other file");
    toolbar->SetToolShortHelp(ID_TOOL_START, "Start scene");
    toolbar->SetToolShortHelp(ID_TOOL_STOP, "Stop scene if started");
    toolbar->SetToolShortHelp(ID_TOOL_ANIMATION_VIEWER, "Start animation viewer tool");
    toolbar->SetToolShortHelp(ID_TOOL_BUILD, "Build projects componets to create shared libs");
    // Separator żeby odsunąć
    toolbar->AddSeparator();

    auto* timeLabel = new wxStaticText(toolbar, wxID_ANY, "Game time: ");
    toolbar->AddControl(timeLabel);

    hourCtrl = new wxSpinCtrl(toolbar, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
    hourCtrl->SetRange(0, 23);
    toolbar->AddControl(hourCtrl);

    minuteCtrl = new wxSpinCtrl(toolbar, wxID_ANY, "0", wxDefaultPosition, wxSize(50, -1));
    minuteCtrl->SetRange(0, 59);
    toolbar->AddControl(minuteCtrl);

    // Suwak godziny (0–24)
    timeSlider = new wxSlider(toolbar, wxID_ANY, 0, 0, 1440, wxDefaultPosition, wxSize(200, -1), wxSL_HORIZONTAL);
    toolbar->AddControl(timeSlider);

    toolbar->Realize();

    timeSlider->Bind(wxEVT_SLIDER,
                     [this](wxCommandEvent& evt)
                     {
                         int minutes      = evt.GetInt();       // 0–1440
                         float normalized = minutes / 1440.0f;  // w [0.0 – 1.0]
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

    DEBUG_LOG("hour: " + std::to_string(hour) + " minute: " + std::to_string(minute));
    timeSlider->SetValue(totalMinutes);
    hourCtrl->SetValue(hour);
    minuteCtrl->SetValue(minute);
}

void MainFrame::AddChilds(GameEngine::GameObject& gameObject, wxTreeItemId parentId)
{
    for (const auto& child : gameObject.GetChildren())
    {
        auto wxItemId = gameObjectsView->AppendItem(parentId, child->GetName(), child->GetId());
        if (isGameObjectPrefab(*child))
        {
            gameObjectsView->DisableItem(wxItemId);
        }
        AddChilds(*child, wxItemId);
    }
}

void MainFrame::OnFileSelectChanged(wxTreeEvent& event)
{
}

void MainFrame::OnFileActivated(const wxString& fullpath)
{
    GameEngine::File file{fullpath.ToStdString()};
    LOG_DEBUG << file;

    auto is3Model = file.IsExtension({"AMF", "3DS",      "AC",      "ASE", "ASSBIN", "B3D",  "BVH",   "COLLADA", "DXF", "CSM",
                                      "DAE", "HMP",      "IRRMESH", "IRR", "LWO",    "LWS",  "MD2",   "MD3",     "MD5", "MD5MESH",
                                      "MDC", "MDL",      "NFF",     "NDO", "OFF",    "OBJ",  "OGRE",  "OPENGEX", "PLY", "MS3D",
                                      "COB", "BLEND",    "IFC",     "XGL", "FBX",    "Q3D",  "Q3BSP", "RAW",     "SIB", "SMD",
                                      "STL", "TERRAGEN", "3D",      "X",   "X3D",    "GLTF", "3MF",   "MMD",     "STEP"});
    if (is3Model)
    {
        auto parentGameObject = GetSelectedGameObject();
        if (auto maybeId = canvas->AddGameObject(file, parentGameObject))
        {
            gameObjectsView->AppendItemToSelection(file.GetBaseName(), *maybeId);
            UpdateObjectCount();
        }
    }
}

void MainFrame::OnObjectTreeSelChange(wxTreeEvent& event)
{
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
        UpdateGameObjectIdOnTransfromLabel(go->GetId());
        worldTransformPanel->set(go->GetWorldTransform());
        localTransformPanel->set(go->GetTransform());

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
            event.Veto();  // Anuluj zmianę
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

            auto parentItem = gameObjectsView->GetItemParent(sel);
            if (parentItem.IsOk())
            {
                gameObjectsView->SelectItem(parentItem);
                if (auto gameObject = GetGameObject(parentItem))
                    UpdateGameObjectIdOnTransfromLabel(gameObject->GetId());
                else
                {
                    UpdateGameObjectIdOnTransfromLabel();
                    worldTransformPanel->lock();
                    localTransformPanel->lock();
                }
            }
            else
            {
                gameObjectsView->SelectItem(gameObjectsView->GetRootItem());
                UpdateGameObjectIdOnTransfromLabel();
                worldTransformPanel->lock();
                localTransformPanel->lock();
            }

            canvas->GetScene().RemoveGameObject(*gameObjectId);
            gameObjectsView->Delete(sel);
            UpdateObjectCount();
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
        wxFileDialog fileDialog(this, "Choose prefab file", Utils::GetAbsolutePath(EngineConf.files.data), "",
                                "Pliki prefabów (*.prefab)|*.prefab", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

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
        auto clonedGo = GameEngine::cloneGameObject(canvas->GetScene(), *maybeGo);

        auto selectedItem = gameObjectsView->GetSelection();
        if (selectedItem.IsOk())
        {
            wxTreeItemId parentItem = gameObjectsView->GetItemParent(selectedItem);
            if (not parentItem.IsOk())
            {
                parentItem = selectedItem;
            }
            auto itemId = gameObjectsView->AppendItem(parentItem, clonedGo->GetName(), clonedGo->GetId());

            std::function<void(wxTreeItemId, GameEngine::GameObject&)> addChildToWidgets;
            addChildToWidgets = [&](wxTreeItemId wxId, GameEngine::GameObject& go)
            {
                const auto& children = go.GetChildren();
                for (const auto& child : children)
                {
                    auto childItemId = gameObjectsView->AppendItem(wxId, child->GetName(), child->GetId());
                    addChildToWidgets(childItemId, *child);
                }
            };
            addChildToWidgets(itemId, *clonedGo);
            UpdateObjectCount();
        }
    }
}

void MainFrame::OnPageChanged(wxBookCtrlEvent& event)
{
    int selection = event.GetSelection();  // indeks nowej aktywnej strony
    if (transfromSubController)
    {
        DEBUG_LOG("selection = " + std::to_string(selection));
        transfromSubController->ChangeState(static_cast<TransfromSubController::State>(selection));
    }
    // ... twoja logika ...
    event.Skip();  // pozwól na dalszą obsługę
}

void MainFrame::ChangeGameObjectParent(GameEngine::GameObject& object, GameEngine::GameObject& newParent)
{
    auto currentParent = object.GetParent();
    if (currentParent and newParent.GetId() != currentParent->GetId())
    {
        auto worldPosition = object.GetWorldTransform().GetPosition();
        auto worldRotation = object.GetWorldTransform().GetRotation();
        auto worldScale    = object.GetWorldTransform().GetScale();

        auto freeGameObject = currentParent->MoveChild(object.GetId());

        if (freeGameObject)
        {
            auto go = freeGameObject.get();
            newParent.MoveChild(std::move(freeGameObject));
            go->SetWorldPosition(worldPosition);
            go->SetWorldRotation(worldRotation);
            go->SetWorldScale(worldScale);
        }
    }
}

void MainFrame::OnToolStart(wxCommandEvent& event)
{
    GameEngine::File sceneFile{"editorTmpSceneToRun.xml"};
    GameEngine::saveSceneToFile(canvas->GetScene(), sceneFile);

    std::string cmd = "\"" + wxStandardPaths::Get().GetExecutablePath().ToStdString() + "\" --scene \"" +
                      sceneFile.GetAbsoultePath() + "\" " + "--projectPath " + ProjectManager::GetInstance().GetProjectPath();

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
    std::string cmd = "\"" + wxStandardPaths::Get().GetExecutablePath().ToStdString() + "\" --animationViewer";

    long pid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_NOHIDE | wxEXEC_NODISABLE);
    if (pid == 0)
    {
        wxLogError("Run AnimationViewer error!");
        return;
    }

    // wxLogMessage("AnimationViewer started, PID=%ld", pid);
}

void MainFrame::OnBuildCmponents(wxCommandEvent&)
{
    LOG_DEBUG << "Build components";

    std::string buildDir          = ProjectManager::GetInstance().GetProjectPath() + "/build";
    std::string engineIncludesDir = ProjectManager::GetInstance().GetEngineIncludesDir();

    if (not std::filesystem::exists(buildDir))
    {
        std::filesystem::create_directories(buildDir);
    }
    if (engineIncludesDir.empty())
    {
        wxLogMessage("engineIncludesDir is empty, check preferenes settings. Should be path to engine repo");
        return;
    }

    LOG_DEBUG << "buildDir : " << buildDir;
    LOG_DEBUG << "engineIncludesDir : " << engineIncludesDir;

    RunCommand("cmake .. -DCOMPONENTS_DIR=Data/Components -DENGINE_INCLUDE_DIR=" + engineIncludesDir, buildDir);
    RunCommand("cmake --build .", buildDir);

    wxLogMessage("Components build finished");
    projectPanel->RefreshAll();
}

void MainFrame::RunCommand(const std::string& cmd, const std::string& workDir)
{
    wxExecuteEnv env;
    env.cwd = workDir;  // katalog roboczy

    long pid = wxExecute(cmd, wxEXEC_SYNC, nullptr, &env);
    if (pid == -1)
    {
        wxLogError("Failed to run command: %s", cmd);
    }
}

bool MainFrame::SaveSceneAs()
{
    wxFileDialog fileDialog(this, "Wybierz plik", Utils::GetAbsolutePath(EngineConf.files.data + "/Scenes"), "",
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
    DEBUG_LOG("SubscribeCurrent");
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
                            gameObject->GetTransform().SetPosition(v);
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
                            gameObject->GetTransform().SetRotation(Rotation{DegreesVec3(v)});
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
                            gameObject->GetTransform().SetScale(v);
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
        DEBUG_LOG("ChangeGameObject " + std::to_string(goId));
        UnsubscribeCurrent();
        gameObjectId = goId;
        SubscribeCurrent();
    }
}

void TransfromSubController::ChangeState(State s)
{
    if (state != s)
    {
        DEBUG_LOG("State " + std::to_string(static_cast<int>(s)));
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
            DEBUG_LOG("Sub world, " + go->GetName());
            subId = go->SubscribeOnWorldTransfomChange(updatePanel);
            transformPanels[state]->set(go->GetWorldTransform());
        }
        else
        {
            DEBUG_LOG("Sub local, " + go->GetName());
            subId = go->GetTransform().SubscribeOnChange(updatePanel);
            transformPanels[state]->set(go->GetTransform());
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
                DEBUG_LOG("Unsub worl, " + go->GetName());
                go->UnsubscribeOnWorldTransfromChange(*subId);
            }
            else
            {
                DEBUG_LOG("Uub local, " + go->GetName());
                go->GetTransform().UnsubscribeOnChange(*subId);
            }
        }
    }

    subId.reset();
}
