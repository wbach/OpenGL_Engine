
#include "MainFrame.h"

#include <GameEngine/Scene/SceneEvents.h>
#include <GameEngine/Scene/SceneUtils.h>
#include <wx/artprov.h>
#include <wx/splitter.h>
#include <wx/statbmp.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <string>

#include "GLCanvas.h"
#include "TransformPanel.h"

namespace
{
enum
{
    ID_MENU_FILE_OPEN_SCENE = 1,
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
    ID_TREE_MENU_3
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
wxEND_EVENT_TABLE()

enum
{
    ID_TOOL_START = wxID_HIGHEST + 1,
    ID_TOOL_STOP
};

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
// clang-format on
{
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
    Bind(wxEVT_MENU, &MainFrame::OnProperties, this, ID_TREE_MENU_3);

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
    wxScrolledWindow* transformView = new wxScrolledWindow(trs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    transformView->SetScrollRate(5, 5);  // ustaw scrollowanie

    wxBoxSizer* transformSizer = new wxBoxSizer(wxVERTICAL);
    transformView->SetSizer(transformSizer);

    // Dodaj kilka paneli TransformPanel (na przykład 3)
    for (int i = 0; i < 3; ++i)
    {
        TransformPanel* panel = new TransformPanel(transformView);
        transformSizer->Add(panel, 0, wxEXPAND | wxALL, 5);
    }

    transformView->Layout();
    transformView->FitInside();

    // Splitter dzielimy na canvas i ten nowy panel z listą
    trs->SplitVertically(canvas, transformView, size.x * 5 / 8);

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
        wxBitmap sampleBitmap = wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_OTHER, wxSize(300, 200));
        filePreview           = new wxStaticBitmap(bottomSpliter, wxID_ANY, sampleBitmap, wxDefaultPosition, wxSize(300, 200));
    }

    bottomSpliter->SplitVertically(fileExplorer, filePreview, size.x / 2);

    CreateMainMenu();
    auto* toolbar = CreateToolBar();
    toolbar->AddTool(ID_TOOL_START, "Start", wxArtProvider::GetBitmap(wxART_GO_FORWARD));
    toolbar->AddTool(ID_TOOL_STOP, "Stop", wxArtProvider::GetBitmap(wxART_CROSS_MARK));
    toolbar->Realize();
    CreateStatusBar();
    SetStatusText("Welcome to game editor!");
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
    auto itemId = gameObjectsView->AppendItem(pranetItemId, name);
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

void MainFrame::MenuEditTerrainHeightPainter(wxCommandEvent&)
{
}

void MainFrame::MenuEditTerrainTexturePainter(wxCommandEvent&)
{
}

void MainFrame::MenuEditLoadPrefab(wxCommandEvent&)
{
}

void MainFrame::MenuEditClearScene(wxCommandEvent&)
{
}

void MainFrame::MenuRendererReloadShaders(wxCommandEvent&)
{
}

void MainFrame::MenuRendererTakeSnapshot(wxCommandEvent&)
{
}

void MainFrame::MenuRendererSwap(wxCommandEvent&)
{
}

void MainFrame::MenuRendererPhysicsVisualization(wxCommandEvent&)
{
}

void MainFrame::MenuRendererNormalsVisualization(wxCommandEvent&)
{
}

void MainFrame::MenuRendererTextureDiffuse(wxCommandEvent&)
{
}

void MainFrame::MenuRendererTextureNormals(wxCommandEvent&)
{
}

void MainFrame::MenuRendererTextureSpecular(wxCommandEvent&)
{
}

void MainFrame::MenuRendererTextureDisplacement(wxCommandEvent&)
{
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
            auto id               = canvas->AddGameObject(file, parentGameObject);
            AddGameObjectToWxWidgets(gameObjectsView->GetSelection(), id, file.GetBaseName());
            UpdateObjectCount();
        }
    }
}

void MainFrame::OnObjectTreeSelChange(wxTreeEvent& event)
{
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
        }
        else
        {
            wxLogMessage("Game object not found in scene");
        }
    }
}

void MainFrame::OnProperties(wxCommandEvent& event)
{
    wxTreeItemId sel = gameObjectsView->GetSelection();
    if (sel.IsOk())
        wxLogMessage("Wlasciwiosci obiektu: %s", gameObjectsView->GetItemText(sel));
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
