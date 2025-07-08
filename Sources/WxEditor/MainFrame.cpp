
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
    ID_ADD_OBJECT,
    ID_OPEN_SCENE,
    ID_FILE_EXPLORER,
    ID_GL_INFO
};
int id = 6;
}  // namespace

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_ADD_OBJECT, MainFrame::OnAddObject)
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
    // splitter->SetAccessible(new SplitterWindowAccessible(splitter));
    wxSplitterWindow* topSplitter   = new wxSplitterWindow(horizontalSpliter, wxID_ANY);
    wxSplitterWindow* bottomSpliter = new wxSplitterWindow(horizontalSpliter, wxID_ANY);
    horizontalSpliter->SplitHorizontally(topSplitter, bottomSpliter, size.y * 3 / 5);

    wxSplitterWindow* trs = new wxSplitterWindow(topSplitter, wxID_ANY);
    // gameObjectsView       = new wxListBox(topSplitter, wxID_ANY);
    // FromDIP(wxSize(160, 250))
    gameObjectsView = new wxTreeCtrl(topSplitter, wxID_ANY, wxPoint(0, 0), wxSize(160, 250), wxTR_DEFAULT_STYLE | wxNO_BORDER);
    treeRootId      = gameObjectsView->AddRoot("Scene", 0);

//    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 1", 0));
//    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 2", 0));
//    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 3", 0));
//    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 4", 0));
//    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 5", 0));

//    int i, count;
//    for (i = 0, count = gameObjectsItemsIds.Count(); i < count; ++i)
//    {
//        wxTreeItemId id = gameObjectsItemsIds.Item(i);
//        gameObjectsView->AppendItem(id, "Subitem 1", 1);
//        gameObjectsView->AppendItem(id, "Subitem 2", 1);
//        gameObjectsView->AppendItem(id, "Subitem 3", 1);
//        gameObjectsView->AppendItem(id, "Subitem 4", 1);
//        gameObjectsView->AppendItem(id, "Subitem 5", 1);
//    }
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

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_ADD_OBJECT, "&Add new object\tCtrl-A", "Add new object");
    menuFile->Append(ID_OPEN_SCENE, "&Open Scene\tCtrl-O", "OpenScene");
    menuFile->Append(ID_EXIT, "&Exit\tCtrl-C", "Close editor");

    wxMenu* menuAbout = new wxMenu;
    menuAbout->Append(ID_GL_INFO, "&GLInfo\tCtrl-H", "Print opengl info");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuAbout, "&About");
    //  menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("Welcome to game editor!");
}

void MainFrame::OnAddObject(wxCommandEvent&)
{
    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item " + std::to_string(id++), 0));
}
void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void MainFrame::OnGLVersion(wxCommandEvent&)
{
    wxLogMessage(canvas->getGlInfo().c_str());
}

void MainFrame::AddChilds(GameEngine::GameObject& gameObject, wxTreeItemId parentId)
{
    for(const auto& child : gameObject.GetChildren())
    {
        auto itemId = gameObjectsView->AppendItem(parentId, child->GetName(), 0);
        //gameObjectsItemsIdsMap.insert({child->GetId(), itemId});
        AddChilds(*child, itemId);
    }
}

void MainFrame::OnOpenScene(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, "Wybierz plik", Utils::GetAbsolutePath(EngineConf.files.data + "/Scenes"), "",
                                "Pliki sceny (*.xml)|*.xml|Wszystkie pliki (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString path = openFileDialog.GetPath();
    canvas->OpenScene(std::string{path.c_str()});

    AddChilds(canvas->GetRootObject(), treeRootId);
//    for(const auto& child : rootObject.GetChildren())
//    {
//        auto itemId = gameObjectsView->AppendItem(treeRootId, child->GetName(), 0);
//        gameObjectsItemsIdsMap.insert({child->GetId(), itemId});
//    }
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
