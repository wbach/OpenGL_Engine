
#include "MainFrame.h"

#include <wx/splitter.h>

#include <string>

#include "GLCanvas.h"

namespace
{
enum
{
    ID_EXIT = 1,
    ID_ADD_OBJECT,
    ID_FILE_EXPLORER,
    ID_GL_INFO
};
int id = 6;
}

// clang-format off
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_ADD_OBJECT, MainFrame::OnAddObject)
    EVT_MENU(ID_GL_INFO, MainFrame::OnGLVersion)
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

    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 1", 0));
    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 2", 0));
    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 3", 0));
    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 4", 0));
    gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, "Item 5", 0));

    int i, count;
    for (i = 0, count = gameObjectsItemsIds.Count(); i < count; ++i)
    {
        wxTreeItemId id = gameObjectsItemsIds.Item(i);
        gameObjectsView->AppendItem(id, "Subitem 1", 1);
        gameObjectsView->AppendItem(id, "Subitem 2", 1);
        gameObjectsView->AppendItem(id, "Subitem 3", 1);
        gameObjectsView->AppendItem(id, "Subitem 4", 1);
        gameObjectsView->AppendItem(id, "Subitem 5", 1);
    }
    topSplitter->SplitVertically(gameObjectsView, trs, size.x / 8);

    canvas                   = new GLCanvas(trs);
    wxListBox* transformView = new wxListBox(trs, wxID_ANY);
    transformView->Append("TransformView");
    trs->SplitVertically(canvas, transformView, size.x * 6 / 8);

    fileExplorer =
        new wxGenericDirCtrl(bottomSpliter, ID_FILE_EXPLORER, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxDefaultSize, 0);

    wxListBox* filePreview = new wxListBox(bottomSpliter, wxID_ANY);
    filePreview->Append("FilePreview");
    bottomSpliter->SplitVertically(fileExplorer, filePreview, size.x / 2);

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_ADD_OBJECT, "&AddNewObject\tCtrl-H", "Add new object");
    menuFile->Append(ID_EXIT, "&Exit\tCtrl-H", "Close editor");

    wxMenu* menuAbout = new wxMenu;
    menuAbout->Append(ID_GL_INFO, "&GLInfo\tCtrl-H", "Print opengl info");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuAbout, "&About");
    //  menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
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
void MainFrame::OnFileSelectChanged(wxTreeEvent& event)
{
    if (fileExplorer)
    {
        //wxLogMessage("Selection changed to \"%s\"", fileExplorer->GetPath(event.GetItem()));
    }
}
void MainFrame::OnFileActivated(wxTreeEvent& event)
{
    if (fileExplorer)
    {
       // wxLogMessage("FileActivated to \"%s\"", fileExplorer->GetPath(event.GetItem()));
        GameEngine::File file{std::string(fileExplorer->GetPath(event.GetItem()).c_str())};
        if (canvas->AddGameObject(file))
        {
            gameObjectsItemsIds.Add(gameObjectsView->AppendItem(treeRootId, file.GetBaseName() + std::to_string(id++), 0));
        }
    }
}
