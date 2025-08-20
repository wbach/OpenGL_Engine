#pragma once

#include <wx/artprov.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/wrapsizer.h>
#include <wx/wx.h>

class ProjectPanel : public wxPanel
{
public:
    ProjectPanel(wxWindow* parent, const wxString& rootPath);

private:
    // Przechowujemy pełną ścieżkę w każdym elemencie drzewa
    struct PathData : public wxTreeItemData
    {
        wxString path;
        explicit PathData(const wxString& p)
            : path(p)
        {
        }
    };

    // UI
    wxTreeCtrl* projectTree{nullptr};
    // wxListCtrl* projectFiles {nullptr};
    wxString rootFolder;

    wxScrolledWindow* filePanel;  // na panel przewijalny
    wxWrapSizer* fileSizer;       // nowy wrap sizer

    // Ikony drzewa (16x16)
    wxImageList* treeImageList{nullptr};
    int treeFolderClosedIdx{-1};
    int treeFolderOpenIdx{-1};

    // Ikony/miniatury listy plików (64x64)
    wxImageList* fileImageList{nullptr};
    int idxFolder{-1};
    int idxDefaultFile{-1};
    wxPanel* selectedItemPanel{nullptr};
    wxWindow* selectedItem{nullptr};
    wxStaticText* selectedLabel{nullptr};

    // Funkcja pomocnicza
    void SelectItem(wxPanel* itemPanel);

    // Inicjalizacja
    void BuildTree();
    void AddSubDirs(const wxTreeItemId& parentId, const wxString& absPath);
    void InitFileList();

    // Odświeżanie zawartości listy
    void RefreshListFor(const wxString& folderPath);

    // Eventy
    void OnTreeSelChanged(wxTreeEvent& e);
    void OnFileActivated(wxListEvent& e);

    void SelectTreeItemByPath(const wxString&);
    wxTreeItemId FindTreeItemByPath(wxTreeItemId, const wxString&);
    void OnThumbClicked(wxMouseEvent& e);

    void CreateFilePanel(wxBoxSizer*);

    wxDECLARE_NO_COPY_CLASS(ProjectPanel);
};
