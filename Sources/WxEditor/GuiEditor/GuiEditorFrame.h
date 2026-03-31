#pragma once
#include <GameEngine/Renderers/GUI/IGuiElementFactory.h>
#include <wx/propgrid/propgrid.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

#include <filesystem>
#include <optional>

#include "Renderers/GUI/Layer/GuiLayer.h"

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
    void AddTreeItem(wxTreeItemId, GameEngine::GuiElement*);

    void OnOpen(wxCommandEvent&);
    void OnSave(wxCommandEvent&);
    void OnSaveAs(wxCommandEvent&);
    void OnExit(wxCommandEvent&);

    void OnUndo(wxCommandEvent&);
    void OnRedo(wxCommandEvent&);
    void OnAddElement(wxCommandEvent&);
    void OnAddElementToLayer(const std::string&, wxCommandEvent&);
    void OnCreateNewLayer(wxCommandEvent&);

    void OnTreeSelectionChanged(wxTreeEvent&);
    void OnPropertyChange(wxPropertyGridEvent&);
    void OnTimer(wxTimerEvent&);

    void OnTreeContextMenu(wxTreeEvent&);
    void OnDeleteElement(GuiTreeItemData&);

private:
    void FocusElementInTree(GameEngine::GuiElement*);
    wxTreeItemId FindItemByElement(wxTreeItemId, GameEngine::GuiElement*);

private:
    wxSplitterWindow* mainSplitter;
    wxSplitterWindow* contentSplitter;
    wxPanel* leftPanel;
    wxTreeCtrl* sceneTree;
    wxPropertyGrid* propGrid;
    GLCanvas* canvas;
    wxTimer* timer;

    GameEngine::IGuiElementFactory* guiFactory = nullptr;
    GameEngine::GuiElement* selectedElement    = nullptr;
    GameEngine::GuiLayer* selectedLayer        = nullptr;
    std::filesystem::path lastDirPath;
};