#pragma once
#include <GameEngine/Renderers/GUI/IElementFactory.h>
#include <GameEngine/Resources/File.h>
#include <wx/propgrid/propgrid.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

#include <filesystem>
#include <optional>

class GLCanvas;
class GuiTreeItemData;

class GuiEditorFrame : public wxFrame
{
public:
    GuiEditorFrame(const std::optional<GameEngine::File>&, const wxString&, const wxPoint&, const wxSize&);

private:
    void Init();
    void CreateMainMenu();
    void RefreshTree();
    void AddTreeItem(wxTreeItemId, GameEngine::GUI::Element*);

    void OnOpen(wxCommandEvent&);
    void OnOpenRecent(wxCommandEvent&);
    void OnReOpen(wxCommandEvent&);
    void OnSave(wxCommandEvent&);
    void OnSaveAs(wxCommandEvent&);
    void OnExit(wxCommandEvent&);

    void OnUndo(wxCommandEvent&);
    void OnRedo(wxCommandEvent&);
    void OnAddElement(wxCommandEvent&);

    void OnTreeSelectionChanged(wxTreeEvent&);
    void OnPropertyChange(wxPropertyGridEvent&);
    void OnTimer(wxTimerEvent&);

    void OnTreeContextMenu(wxTreeEvent&);
    void OnDeleteElement(GuiTreeItemData&);

    void MoveTreeItem(wxTreeItemId, wxTreeItemId);

private:
    void FocusElementInTree(GameEngine::GUI::Element*);
    wxTreeItemId FindItemByElement(wxTreeItemId, GameEngine::GUI::Element*);
    void ReadRecentOpenedFiles();
    void SaveRecentFiles();
    void AddToRecentFile(const GameEngine::File&);
    void UpdateRecentFilesMenu();
    bool OpenFile(const GameEngine::File&);
    void commonProperties(wxPropertyGrid&, GameEngine::GUI::Element&);
    void renderAbleProperties(wxPropertyGrid&, GameEngine::GUI::Element&);
    void windowProperties(wxPropertyGrid&, GameEngine::GUI::Element&);
    void buttonProperties(wxPropertyGrid&, GameEngine::GUI::Element&);
    void textProperties(wxPropertyGrid&, GameEngine::GUI::Element&);
    void multiLineTextProperties(wxPropertyGrid&, GameEngine::GUI::Element&);
    void spriteProperties(wxPropertyGrid&, GameEngine::GUI::Element&, wxPGProperty* = nullptr);
    void layoutProperties(wxPropertyGrid&, GameEngine::GUI::Element&);
    void verticalProperties(wxPropertyGrid&, GameEngine::GUI::Element&);
    void rebuildPropertiesView();

private:
    wxSplitterWindow* mainSplitter;
    wxSplitterWindow* contentSplitter;
    wxPanel* leftPanel;
    wxTreeCtrl* sceneTree;
    wxPropertyGrid* propGrid;
    GLCanvas* canvas;
    wxTimer* timer;
    wxMenu* recentMenu;

    GameEngine::GUI::IElementFactory* guiFactory = nullptr;
    GameEngine::GUI::Element* selectedElement    = nullptr;
    std::filesystem::path lastDirPath;
    std::optional<GameEngine::File> currentFile;
    std::vector<std::filesystem::path> recentFiles;
};