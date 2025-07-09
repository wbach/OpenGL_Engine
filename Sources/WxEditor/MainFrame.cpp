
#include "MainFrame.h"

#include <wx/splitter.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <string>

#include "GLCanvas.h"

namespace
{
enum
{
    ID_EXIT = 1,
    ID_CREATE_OBJECT,
    ID_CREATE_TERRAIN,
    ID_TERRAIN_HEIGHT_PAINTER,
    ID_TERRAIN_TEXTURE_PAINTER,
    ID_LOAD_PREFAB,
    ID_OPEN_SCENE,
    ID_RELOAD_SCENE,
    ID_SAVE_SCENE,
    ID_SAVEAS_SCENE,
    ID_CLEAR_SCENE,
    ID_FILE_EXPLORER,
    ID_RELOAD_SHADERS,
    ID_TAKE_RENDERER_SNAPSHOT,
    ID_GL_INFO
};
int id = 6;
}  // namespace

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_CREATE_OBJECT, MainFrame::OnCreateObject)
    EVT_MENU(ID_GL_INFO, MainFrame::OnGLVersion)
    EVT_MENU(ID_OPEN_SCENE, MainFrame::OnOpenScene)
    EVT_MENU(ID_EXIT, MainFrame::OnExit)
    EVT_DIRCTRL_SELECTIONCHANGED(ID_FILE_EXPLORER, MainFrame::OnFileSelectChanged)
    EVT_DIRCTRL_FILEACTIVATED(ID_FILE_EXPLORER, MainFrame::OnFileActivated)
wxEND_EVENT_TABLE()


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
    gameObjectsView = new wxTreeCtrl(topSplitter, wxID_ANY, wxPoint(0, 0), wxSize(160, 250), wxTR_DEFAULT_STYLE | wxNO_BORDER);

    CreateRootGameObject();

    topSplitter->SplitVertically(gameObjectsView, trs, size.x / 8);

    canvas                   = new GLCanvas(trs);
    wxListBox* transformView = new wxListBox(trs, wxID_ANY);
    transformView->Append("TransformView");
    trs->SplitVertically(canvas, transformView, size.x * 6 / 8);

    fileExplorer = new wxGenericDirCtrl(bottomSpliter, ID_FILE_EXPLORER, Utils::GetAbsolutePath(EngineConf.files.data),
                                        wxDefaultPosition, wxDefaultSize, wxDIRCTRL_SELECT_FIRST);

    wxListBox* filePreview = new wxListBox(bottomSpliter, wxID_ANY);
    filePreview->Append("FilePreview");
    bottomSpliter->SplitVertically(fileExplorer, filePreview, size.x / 2);

    CreateMainMenu();
    CreateStatusBar();
    SetStatusText("Welcome to game editor!");
}

void MainFrame::OnCreateObject(wxCommandEvent&)
{
    // gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item " + std::to_string(id++), 0));
}
void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
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
    wxMenu* menuAbout = new wxMenu;
    menuAbout->Append(ID_GL_INFO, "&GLInfo\tCtrl-H", "Print opengl info");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(CreateFileMenu(), "&File");
    menuBar->Append(CreateEditMenu(), "&Edit");
    menuBar->Append(CreateRendererMenu(), "&Renderer");
    menuBar->Append(menuAbout, "&About");
    //  menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
}

wxMenu* MainFrame::CreateFileMenu()
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_OPEN_SCENE, "&Open scene\tCtrl-O", "OpenScene");
    menuFile->Append(ID_RELOAD_SCENE, "&Reload scene\tCtrl-O", "OpenScene");
    menuFile->Append(ID_SAVE_SCENE, "&Save scene\tCtrl-S", "SaveScene");
    menuFile->Append(ID_SAVEAS_SCENE, "&Save scene as\tCtrl-S", "SaveScene");
    menuFile->Append(ID_EXIT, "&Exit\tCtrl-C", "Close editor");
    return menuFile;
}

wxMenu* MainFrame::CreateEditMenu()
{
    wxMenu* menu = new wxMenu;
    menu->Append(ID_CREATE_OBJECT, "&Create new object\tCtrl-A", "Create empty new object");
    menu->Append(ID_CREATE_TERRAIN, "&Create terrain\tCtrl-A", "Create gameobject with terrain components");
    menu->Append(ID_TERRAIN_HEIGHT_PAINTER, "&Terrain height painter \tCtrl-A", "Enable height painter tool");
    menu->Append(ID_TERRAIN_TEXTURE_PAINTER, "&Terrain texture painter \tCtrl-A", "Enable terrain texture painter tool");
    menu->Append(ID_LOAD_PREFAB, "&Load from prefab\tCtrl-A", "Create new object");
    menu->Append(ID_CLEAR_SCENE, "&Clear\tCtrl-A", "Delete all object in scene");
    return menu;
}

wxMenu* MainFrame::CreateRendererMenu()
{
    wxMenu* menu = new wxMenu;
    menu->Append(ID_RELOAD_SHADERS, "&Reload shaders\tCtrl-A", "Reload current shaders");
    menu->Append(ID_TAKE_RENDERER_SNAPSHOT, "&Take snapshot\tCtrl-A", "Create snapshot of renderer state");
    menu->Append(ID_TAKE_RENDERER_SNAPSHOT, "&Swap render mode\tCtrl-A", "Switch between line and fill render mode");
    menu->Append(ID_TAKE_RENDERER_SNAPSHOT, "&Visualize physics\tCtrl-A", "Enable/Disable of physics visualization");
    menu->Append(ID_TAKE_RENDERER_SNAPSHOT, "&Visualize normals\tCtrl-A", "Enable/Disable of normals visualization");

    wxMenu* texturesMenu = new wxMenu;
    texturesMenu->Append(ID_TAKE_RENDERER_SNAPSHOT, "&Diffuse\tCtrl-A", "Enable/Disable of normals visualization");
    texturesMenu->Append(ID_TAKE_RENDERER_SNAPSHOT, "&Normals\tCtrl-A", "Enable/Disable of normals visualization");
    texturesMenu->Append(ID_TAKE_RENDERER_SNAPSHOT, "&Specular\tCtrl-A", "Enable/Disable of normals visualization");
    texturesMenu->Append(ID_TAKE_RENDERER_SNAPSHOT, "&Displacment\tCtrl-A", "Enable/Disable of normals visualization");

    menu->AppendSubMenu(texturesMenu, "&Texture rendering\tCtrl-A", "Enable/Disable of normals visualization");
    return menu;
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
        auto itemId = gameObjectsView->AppendItem(parentId, name, 0);
        // gameObjectsItemsIdsMap.insert({child->GetId(), itemId});
        AddChilds(*child, itemId);
    }
}

void MainFrame::OnOpenScene(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, "Wybierz plik", Utils::GetAbsolutePath(EngineConf.files.data + "/Scenes"), "",
                                "Pliki sceny (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    gameObjectsView->DeleteAllItems();
    CreateRootGameObject();
    wxString path = openFileDialog.GetPath();
    canvas->OpenScene(std::string{path.c_str()});

    AddChilds(canvas->GetRootObject(), treeRootId);

    auto objectCount = gameObjectsView->GetChildrenCount(treeRootId);
    gameObjectsView->SetItemText(treeRootId, "Scene (Objects: " + std::to_string(objectCount) + ")");
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
        if (is3Model && canvas->AddGameObject(file))
        {
            gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, file.GetBaseName() + std::to_string(id++), 0));
        }
    }
}
