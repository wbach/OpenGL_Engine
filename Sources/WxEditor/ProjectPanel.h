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

#include <functional>
#include <memory>

class ProjectPanel : public wxPanel
{
public:
    using FileSelectedCallback = std::function<void(const wxString&)>;
    ProjectPanel(wxWindow* parent, const wxString& rootPath, FileSelectedCallback callback);

    // --- Odświeżanie ---
    void RefreshListFor(const wxString& folderPath);
    void RefreshListAndTreeFor(const wxString& folderPath);
    void RebuildTreeAndSelect(const wxString& folderPath)
    {
               BuildTree();
            SelectTreeItemByPath(folderPath);
    }

    wxString GetCurrentFolderPath() const;

private:
    FileSelectedCallback fileSelectedCallback;

    struct PathData : public wxTreeItemData
    {
        wxString path;
        explicit PathData(const wxString& p)
            : path(p)
        {
        }
    };

    struct FileInfo
    {
        wxFileName name;
        wxStaticBitmap* icon{nullptr};
        wxStaticText* label{nullptr};
    };

    void addContextMenu(wxWindow*);
    void addContextMenu(wxWindow*, const FileInfo&);
    void contextMenuTriggerAction(wxMouseEvent&, wxWindow*, const wxFileName&, bool = true);

private:
    wxTreeCtrl* projectTree{nullptr};
    wxString rootFolder;

    // --- Dane panelu plików ---
    wxScrolledWindow* filePanel{nullptr};
    wxWrapSizer* fileSizer{nullptr};
    wxPoint dragStartPos;
    wxStaticBitmap* dragIcon{nullptr};

    // Wybrany element
    wxPanel* selectedItemPanel{nullptr};
    wxWindow* selectedItem{nullptr};
    wxStaticText* selectedLabel{nullptr};
    wxString currentFolderPath;

    // --- Ikony ---
    // Drzewo (16x16)
    wxImageList* treeImageList{nullptr};
    int treeFolderClosedIdx{-1};
    int treeFolderOpenIdx{-1};

    // Pliki / miniaturki (64x64)
    wxImageList* fileImageList{nullptr};
    int idxFolder{-1};
    int idxDefaultFile{-1};

    // --- Funkcje pomocnicze ---
    void SelectItem(wxPanel* itemPanel);
    void SelectItem(wxWindow* item, wxStaticText* label);
    void UnSelectCurrentItem();

    // --- Inicjalizacja UI ---
    void BuildTree();
    void AddSubDirs(const wxTreeItemId& parentId, const wxString& absPath);
    void InitFileList();
    void CreateFilePanel(wxBoxSizer* sizer);
    wxBoxSizer* CreateFileItem(const wxFileName& fn, const wxBitmap& bmp, bool selectable, const std::function<void()>& callback);

    // --- Eventy ---
    void OnTreeSelChanged(wxTreeEvent& e);
    void OnFileActivated(wxListEvent& e);

    void SelectTreeItemByPath(const wxString&);
    wxTreeItemId FindTreeItemByPath(wxTreeItemId parent, const wxString& path);

    // --- Miniaturki ---
    wxBitmap GetThumbnail(const wxFileName& fn, int thumbSize);
    wxDECLARE_NO_COPY_CLASS(ProjectPanel);
};
