#include "MainFrame.h"

#include <GameEngine/DebugTools/Painter/TerrainHeightPainter.h>
#include <GameEngine/DebugTools/Painter/TerrainTexturePainter.h>
#include <GameEngine/Scene/SceneEvents.h>
#include <GameEngine/Scene/SceneReader.h>
#include <GameEngine/Scene/SceneUtils.h>
#include <wx/artprov.h>
#include <wx/splitter.h>
#include <wx/statbmp.h>
#include <wx/spinctrl.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <string>

#include "GLCanvas.h"
#include "OptionsFrame.h"
#include "TransformPanel.h"
#include "ComponentPanel.h"

namespace
{
enum
{
    ID_MENU_FILE_OPEN_SCENE = wxID_HIGHEST + 1,
    ID_MENU_FILE_RELOAD_SCENE,
    ID_MENU_FILE_SAVE_SCENE,
    ID_MENU_FILE_SAVEAS_SCENE,
    ID_MENU_FILE_EXIT,
    ID_MENU_EDIT_CREATE_OBJECT,
    ID_MENU_EDIT_CREATE_TERRAIN,
    ID_MENU_EDIT_TERRAIN_HEIGHT_PAINTER,
    ID_MENU_EDIT_TERRAIN_TEXTURE_PAINTER,
    ID_MENU_EDIT_LOAD_PREFAB,
    ID_MENU_EDIT_CLEAR_SCENE,
    ID_MENU_EDIT_PREFERENCES,
    ID_MENU_RENDERER_RELOAD_SHADERS,
    ID_MENU_RENDERER_TAKE_RENDERER_SNAPSHOT,
    ID_MENU_RENDERER_SWAP,
    ID_MENU_RENDERER_PHYSICS_VISUALIZATION,
    ID_MENU_RENDERER_NORMAL_VISUALIZATION,
    ID_MENU_RENDERER_TEXTURE_DIFFUSE,
    ID_MENU_RENDERER_TEXTURE_NORMALS,
    ID_MENU_RENDERER_TEXTURE_SPECULAR,
    ID_MENU_RENDERER_TEXTURE_DISPLACEMENT,
    ID_FILE_EXPLORER,
    ID_OBJECT_TREE,
    ID_MENU_ABOUT_GL_INFO,
    ID_TREE_MENU_CREATE_CHILD,
    ID_TREE_MENU_REMOVE,
    ID_TREE_MENU_CLONE,
    ID_SAVE,
    ID_TOOL_OPEN,
    ID_TOOL_SAVE,
    ID_TOOL_SAVE_AS,
    ID_TOOL_START,
    ID_TOOL_STOP

};
}  // namespace


// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
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
    EVT_TREE_BEGIN_DRAG(ID_OBJECT_TREE, MainFrame::OnObjectDrag)
    EVT_TREE_END_DRAG(ID_OBJECT_TREE, MainFrame::OnObjectEndDrag)
    EVT_TREE_BEGIN_LABEL_EDIT(ID_OBJECT_TREE, MainFrame::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(ID_OBJECT_TREE, MainFrame::OnEndLabelEdit)
    EVT_DIRCTRL_SELECTIONCHANGED(ID_FILE_EXPLORER, MainFrame::OnFileSelectChanged)
    EVT_DIRCTRL_FILEACTIVATED(ID_FILE_EXPLORER, MainFrame::OnFileActivated)
    EVT_MENU(ID_TOOL_OPEN, MainFrame::MenuFileOpenScene)
    EVT_MENU(ID_TOOL_SAVE, MainFrame::MenuFileSaveScene)
    EVT_MENU(ID_TOOL_SAVE_AS, MainFrame::MenuFileSaveSceneAs)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
// clang-format on
{
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxJPEGHandler);
    wxImage::AddHandler(new wxBMPHandler);

    wxSplitterWindow* horizontalSpliter = new wxSplitterWindow(this, wxID_ANY);
    wxSplitterWindow* topSplitter       = new wxSplitterWindow(horizontalSpliter, wxID_ANY);
    wxSplitterWindow* bottomSpliter     = new wxSplitterWindow(horizontalSpliter, wxID_ANY);
    horizontalSpliter->SplitHorizontally(topSplitter, bottomSpliter, size.y * 3 / 5);

    wxSplitterWindow* trs = new wxSplitterWindow(topSplitter, wxID_ANY);
    // FromDIP(wxSize(160, 250))
    gameObjectsView = new wxTreeCtrl(topSplitter, ID_OBJECT_TREE, wxPoint(0, 0), wxSize(160, 250),
                                     wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_EDIT_LABELS);
    gameObjectsView->Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &MainFrame::OnTreeItemRightClick, this);
    Bind(wxEVT_MENU, &MainFrame::OnAddObject, this, ID_TREE_MENU_CREATE_CHILD);
    Bind(wxEVT_MENU, &MainFrame::OnDeleteObject, this, ID_TREE_MENU_REMOVE);
    Bind(wxEVT_MENU, &MainFrame::CloneGameObject, this, ID_TREE_MENU_CLONE);

    CreateRootGameObject();

    topSplitter->SplitVertically(gameObjectsView, trs, size.x / 8);

    canvas = new GLCanvas(trs,
                          [&](uint32 gameObjectId, bool select)
                          {
                              for (const auto& [wxItemId, goId] : gameObjectsItemsIdsMap)
                              {
                                  if (goId == gameObjectId)
                                  {
                                      gameObjectsView->SelectItem(wxItemId, select);
                                  }
                              }
                          });

    // Zamiast wxListBox
    gameObjectPanels = new wxScrolledWindow(trs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    gameObjectPanels->SetScrollRate(5, 5);  // ustaw scrollowanie

    gameObjectPanelsSizer = new wxBoxSizer(wxVERTICAL);
    gameObjectPanels->SetSizer(gameObjectPanelsSizer);

    transformsNotebook = new wxNotebook(gameObjectPanels, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    transformsNotebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &MainFrame::OnPageChanged, this);

    worldTransformPanel = new TransformPanel(transformsNotebook);
    transformsNotebook->AddPage(worldTransformPanel, "World");
    localTransformPanel = new TransformPanel(transformsNotebook);
    transformsNotebook->AddPage(localTransformPanel, "Local");

    gameObjectPanelsSizer->Add(transformsNotebook, 0, wxEXPAND | wxALL, 5);

    gameObjectPanels->Layout();
    gameObjectPanels->FitInside();

    // Splitter dzielimy na canvas i ten nowy panel z listą
    trs->SplitVertically(canvas, gameObjectPanels, size.x * 5 / 8);

    fileExplorer = new wxGenericDirCtrl(bottomSpliter, ID_FILE_EXPLORER, Utils::GetAbsolutePath(EngineConf.files.data),
                                        wxDefaultPosition, wxDefaultSize, wxDIRCTRL_SELECT_FIRST);

    wxStaticBitmap* filePreview = nullptr;
    //    wxImage img;
    //    if (img.LoadFile("sample.png", wxBITMAP_TYPE_PNG))
    //    {
    //        wxBitmap bmp(img);
    //        filePreview = new wxStaticBitmap(bottomSpliter, wxID_ANY, bmp, wxDefaultPosition, wxSize(300, 200));
    //    }
    //    else
    {
        wxBitmap sampleBitmap = wxArtProvider::GetBitmap(wxART_MISSING_IMAGE, wxART_OTHER, wxSize(300, 200));
        filePreview           = new wxStaticBitmap(bottomSpliter, wxID_ANY, sampleBitmap, wxDefaultPosition, wxSize(300, 200));
    }

    bottomSpliter->SplitVertically(fileExplorer, filePreview, size.x / 2);

    CreateMainMenu();
    auto* toolbar = CreateToolBar();

    toolbar->AddTool(ID_TOOL_OPEN, "Open", wxArtProvider::GetBitmap(wxART_FILE_OPEN));
    toolbar->AddTool(ID_TOOL_SAVE, "Save", wxArtProvider::GetBitmap(wxART_FILE_SAVE));
    toolbar->AddTool(ID_TOOL_SAVE_AS, "SaveAs", wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS));
    toolbar->AddTool(ID_TOOL_START, "Start", wxArtProvider::GetBitmap(wxART_GO_FORWARD));
    toolbar->AddTool(ID_TOOL_STOP, "Stop", wxArtProvider::GetBitmap(wxART_CROSS_MARK));
    toolbar->Realize();
    CreateStatusBar();
    SetStatusText("Welcome to game editor!");

    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, (int)'S', ID_SAVE);  // ID_SAVE to Twój identyfikator

    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    // Powiązanie zdarzenia z ID_SAVE
    Bind(wxEVT_MENU, &MainFrame::MenuFileSaveScene, this, ID_SAVE);
}

void MainFrame::RemoveAllItemsButTransformView()
{
    if (not gameObjectPanelsSizer or not transformsNotebook)
        return;

    for (size_t i = 0; i < gameObjectPanelsSizer->GetItemCount(); /* no increment */)
    {
        wxSizerItem* item = gameObjectPanelsSizer->GetItem(i);
        if (not item)
            continue;

        if (item->GetWindow() == transformsNotebook)
        {
            i++;
            continue;
        }

        // Najpierw usuń okno lub sizer
        if (item->GetWindow())
            item->GetWindow()->Destroy();
        else if (item->GetSizer())
            item->GetSizer()->Clear(true);

        // Usuń element z sizera — wxWidgets samo zwolni wxSizerItem
        gameObjectPanelsSizer->Remove(i);
    }

    gameObjectPanelsSizer->Layout();
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

void MainFrame::MenuFileOpenScene(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, "Wybierz plik", Utils::GetAbsolutePath(EngineConf.files.data + "/Scenes"), "",
                                "Pliki sceny (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    canvas->ResetDragObject();
    gameObjectsView->DeleteAllItems();
    wxString path = openFileDialog.GetPath();
    CreateRootGameObject();
    GameEngine::File file{std::string{path.c_str()}};
    canvas->OpenScene(file,
                      [&]()
                      {
                          if (isRunning)
                          {
                              AddChilds(canvas->GetRootObject(), treeRootId);
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

void MainFrame::MenuFileSaveScene(wxCommandEvent& e)
{
    if (not canvas->GetScene().GetFile().empty())
    {
        GameEngine::saveSceneToFile(canvas->GetScene());
        return;
    }

    MenuFileSaveSceneAs(e);
}

void MainFrame::MenuFileSaveSceneAs(wxCommandEvent&)
{
    wxFileDialog fileDialog(this, "Wybierz plik", Utils::GetAbsolutePath(EngineConf.files.data + "/Scenes"), "",
                            "Pliki sceny (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*", wxFD_SAVE);

    if (fileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString path = fileDialog.GetPath();
    GameEngine::File file{path.c_str()};
    canvas->GetScene().ChangeName(file.GetBaseName());
    SetTitle("Active scene : " + canvas->GetScene().GetName());
    GameEngine::saveSceneToFile(canvas->GetScene(), GameEngine::File{file});
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
    AddGameObjectToWxWidgets(gameObjectsView->GetSelection(), gameObject->GetId(), gameObject->GetName());
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

wxTreeItemId MainFrame::AddGameObjectToWxWidgets(wxTreeItemId pranetItemId, IdType goId, const std::string& name)
{
    auto isPrefab = false;
    if (auto go = canvas->GetScene().GetGameObject(goId))
    {
        isPrefab = go->isPrefabricated();
    }

    auto goName = name;
    if (isPrefab)
    {
        goName += " (prefab)";
    }

    auto itemId = gameObjectsView->AppendItem(pranetItemId, goName);
    gameObjectsItemsIdsMap.insert({itemId, goId});
    return itemId;
}

void MainFrame::UpdateObjectCount()
{
    auto objectCount = gameObjectsView->GetChildrenCount(treeRootId);
    gameObjectsView->SetItemText(treeRootId, "Scene (Objects: " + std::to_string(objectCount) + ")");
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
    auto goIter = gameObjectsItemsIdsMap.find(id);
    if (goIter != gameObjectsItemsIdsMap.end())
    {
        return canvas->GetScene().GetGameObject(goIter->second);
    }
    return nullptr;
}

std::optional<IdType> MainFrame::GetGameObjectId(wxTreeItemId id)
{
    auto goIter = gameObjectsItemsIdsMap.find(id);
    if (goIter != gameObjectsItemsIdsMap.end())
    {
        return goIter->second;
    }
    return std::nullopt;
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

void MainFrame::AddGameObjectComponentsToView(const GameEngine::GameObject& gameObject)
{
    for (auto& component : gameObject.GetComponents())
    {
        ComponentPanel* compPanel = new ComponentPanel(gameObjectPanels);
        compPanel->AddComponent(*component);
        gameObjectPanelsSizer->Add(compPanel, 0, wxEXPAND | wxALL, 0);
    }

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
    wxFileDialog openFileDialog(this, "Wybierz plik", Utils::GetAbsolutePath(EngineConf.files.data + "/Scenes"), "",
                                "Pliki prefab (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    auto go = GameEngine::SceneReader::loadPrefab(canvas->GetScene(), openFileDialog.GetPath().ToStdString());
    if (go)
    {
        auto prefabItemId = AddGameObjectToWxWidgets(treeRootId, go->GetId(), go->GetName());

        // TO DO remove duplicate
        std::function<void(wxTreeItemId, GameEngine::GameObject&)> addChildToWidgets;
        addChildToWidgets = [&](wxTreeItemId wxId, GameEngine::GameObject& go)
        {
            const auto& children = go.GetChildren();
            for (const auto& child : children)
            {
                auto childItemId = AddGameObjectToWxWidgets(wxId, child->GetId(), child->GetName());
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
        canvas->GetScene().ClearGameObjects();
        gameObjectsView->DeleteAllItems();
        gameObjectsItemsIdsMap.clear();
        CreateRootGameObject();
        canvas->ResetDragObject();
        transfromSubController.reset();
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

void MainFrame::CreateRootGameObject()
{
    treeRootId = gameObjectsView->AddRoot("Scene (Objects: 0)", 0);
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
    menuFile->Append(ID_MENU_FILE_OPEN_SCENE, "&Open scene\tCtrl-O", "OpenScene");
    menuFile->Append(ID_MENU_FILE_RELOAD_SCENE, "&Reload scene\tCtrl-O", "OpenScene");
    menuFile->Append(ID_MENU_FILE_SAVE_SCENE, "&Save scene\tCtrl-S", "SaveScene");
    menuFile->Append(ID_MENU_FILE_SAVEAS_SCENE, "&Save scene as\tCtrl-S", "SaveScene");
    menuFile->Append(ID_MENU_FILE_EXIT, "&Exit\tCtrl-C", "Close editor");
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

void MainFrame::AddChilds(GameEngine::GameObject& gameObject, wxTreeItemId parentId)
{
    for (const auto& child : gameObject.GetChildren())
    {
        auto name = child->GetName();
        if (child->isPrefabricated())
        {
            name += " (prefab)";
        }
        auto wxItemId = AddGameObjectToWxWidgets(parentId, child->GetId(), name);
        AddChilds(*child, wxItemId);
    }
}

void MainFrame::OnFileSelectChanged(wxTreeEvent& event)
{
    if (fileExplorer)
    {
        // wxLogMessage("Selection changed to \"%s\"", fileExplorer->GetPath(event.GetItem()));
        // Load preview if can
    }
}
void MainFrame::OnFileActivated(wxTreeEvent& event)
{
    if (fileExplorer)
    {
        // wxLogMessage("FileActivated to \"%s\"", fileExplorer->GetPath(event.GetItem()));
        GameEngine::File file{std::string(fileExplorer->GetPath(event.GetItem()).c_str())};

        auto is3Model =
            file.IsExtension({"AMF", "3DS",      "AC",      "ASE", "ASSBIN", "B3D",  "BVH",   "COLLADA", "DXF", "CSM",
                              "DAE", "HMP",      "IRRMESH", "IRR", "LWO",    "LWS",  "MD2",   "MD3",     "MD5", "MD5MESH",
                              "MDC", "MDL",      "NFF",     "NDO", "OFF",    "OBJ",  "OGRE",  "OPENGEX", "PLY", "MS3D",
                              "COB", "BLEND",    "IFC",     "XGL", "FBX",    "Q3D",  "Q3BSP", "RAW",     "SIB", "SMD",
                              "STL", "TERRAGEN", "3D",      "X",   "X3D",    "GLTF", "3MF",   "MMD",     "STEP"});
        if (is3Model)
        {
            auto parentGameObject = GetSelectedGameObject();
            if (auto maybeId = canvas->AddGameObject(file, parentGameObject))
            {
                AddGameObjectToWxWidgets(gameObjectsView->GetSelection(), *maybeId, file.GetBaseName());
                UpdateObjectCount();
            }
        }
    }
}

void MainFrame::OnObjectTreeSelChange(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    if (not itemId.IsOk())
        return;

    if (itemId == treeRootId)
        return;

    DEBUG_LOG("OnObjectTreeSelChange");
    auto go = GetSelectedGameObject();
    if (go)
    {
        DEBUG_LOG("OnObjectTreeSelChange go: " + go->GetName());
        worldTransformPanel->set(go->GetWorldTransform());
        localTransformPanel->set(go->GetTransform());

        if (not transfromSubController)
        {
            transfromSubController.emplace(*canvas, worldTransformPanel, localTransformPanel, go->GetId());
        }
        else
        {
            transfromSubController->ChangeGameObject(go->GetId());
        }
        RemoveAllItemsButTransformView();
        AddGameObjectComponentsToView(*go);
    }
}

void MainFrame::OnObjectTreeActivated(wxTreeEvent& event)
{
    auto iter = gameObjectsItemsIdsMap.find(event.GetItem().GetID());
    if (iter != gameObjectsItemsIdsMap.end())
    {
        const auto& [_, goId] = *iter;
        auto& scene           = canvas->GetScene();

        if (auto gameObject = scene.GetGameObject(goId))
        {
            scene.GetCamera().SetPosition(gameObject->GetWorldTransform().GetPosition() +
                                          (gameObject->GetWorldTransform().GetScale() + vec3(1.f)));
            scene.GetCamera().LookAt(gameObject->GetWorldTransform().GetPosition());
        }
    }
}

void MainFrame::OnTreeItemRightClick(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    if (!itemId.IsOk())
        return;

    treeRightClickedItem = event.GetItem();
    gameObjectsView->SelectItem(itemId);

    wxMenu menu;
    menu.Append(ID_TREE_MENU_CREATE_CHILD, "Create child");
    menu.Append(ID_TREE_MENU_REMOVE, "Remove");
    menu.Append(ID_TREE_MENU_CLONE, "Clone");
    //    menu.AppendSeparator();
    //    menu.Append(ID_TREE_MENU_3, "Properties");
    PopupMenu(&menu);
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

void MainFrame::OnAddObject(wxCommandEvent& event)
{
    MenuEditCreateObject(event);
}

void MainFrame::OnDeleteObject(wxCommandEvent& event)
{
    wxTreeItemId sel = gameObjectsView->GetSelection();
    if (sel.IsOk() and sel.GetID() != treeRootId)
    {
        auto gameObjectId = GetGameObjectId(sel);
        if (gameObjectId)
        {
            canvas->GetScene().RemoveGameObject(*gameObjectId);
            gameObjectsView->Delete(sel);
            UpdateObjectCount();
        }
        else
        {
            wxLogMessage("Game object not found in scene");
        }
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
            if (parentItem.IsOk())
            {
                selectedItem = parentItem;
            }
            auto itemId = AddGameObjectToWxWidgets(selectedItem, clonedGo->GetId(), clonedGo->GetName());

            std::function<void(wxTreeItemId, GameEngine::GameObject&)> addChildToWidgets;
            addChildToWidgets = [&](wxTreeItemId wxId, GameEngine::GameObject& go)
            {
                const auto& children = go.GetChildren();
                for (const auto& child : children)
                {
                    auto childItemId = AddGameObjectToWxWidgets(wxId, child->GetId(), child->GetName());
                    addChildToWidgets(childItemId, *child);
                }
            };
            addChildToWidgets(itemId, *clonedGo);
            UpdateObjectCount();
        }
    }
}

void MainFrame::OnObjectDrag(wxTreeEvent& event)
{
    if (not event.GetItem().IsOk())
        return;

    treeDragItemId = event.GetItem();
    event.Allow();
}

void MainFrame::OnObjectEndDrag(wxTreeEvent& event)
{
    auto target = event.GetItem();
    if (not target.IsOk() or not treeDragItemId.IsOk())
        return;

    if (target == treeDragItemId)
        return;

    auto dragedGameObject = GetGameObject(treeDragItemId);
    auto newParent = target == gameObjectsView->GetRootItem() ? &canvas->GetScene().GetRootGameObject() : GetGameObject(target);

    if (dragedGameObject and newParent)
    {
        DEBUG_LOG("Change gameObjectParent");
        ChangeGameObjectParent(*dragedGameObject, *newParent);
    }

    auto text    = gameObjectsView->GetItemText(treeDragItemId);
    auto newItem = gameObjectsView->AppendItem(target, text);

    std::optional<IdType> maybeGameObjectIdInMap;
    auto goIter = gameObjectsItemsIdsMap.find(treeDragItemId);
    if (goIter != gameObjectsItemsIdsMap.end())
    {
        maybeGameObjectIdInMap = goIter->second;
    }

    if (maybeGameObjectIdInMap)
    {
        gameObjectsItemsIdsMap.erase(treeDragItemId);
        gameObjectsItemsIdsMap.insert({newItem, *maybeGameObjectIdInMap});
    }

    gameObjectsView->Delete(treeDragItemId);
    treeDragItemId = {};
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
