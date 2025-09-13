#include "ProjectPanel.h"

#include <Logger/Log.h>
#include <wx/clipbrd.h>
#include <wx/dnd.h>
#include <wx/renderer.h>
#include <wx/stdpaths.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <cstdio>
#include <filesystem>
#include <string>

#include "EditorUitls.h"
#include "Resources/File.h"
#include "ThumbnailCache.h"
#include "model3d_icon.h"

namespace
{
class FileDropTarget : public wxFileDropTarget
{
public:
    FileDropTarget(ProjectPanel* panel)
        : m_panel(panel)
    {
    }

    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override
    {
        if (!m_panel)
            return false;

        wxString destFolder = m_panel->GetCurrentFolderPath();

        for (auto& srcPath : filenames)
        {
            Utils::CopyFileOrFolder(srcPath.ToStdString(), destFolder.ToStdString());
        }

        // Odśwież listę tylko w aktualnym folderze
        m_panel->RefreshAll(destFolder);
        return true;
    }

private:
    ProjectPanel* m_panel;
};

void ShowProperties(const wxFileName& fn)
{
    wxDialog dlg(nullptr, wxID_ANY, "Properties");

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxString info;
    info << "Name: " << fn.GetFullName() << "\n"
         << "Path: " << fn.GetFullPath() << "\n"
         << "Type: " << (fn.IsDir() ? "Folder" : "File") << "\n";

    if (!fn.IsDir())
    {
        info << "Size: " << fn.GetSize() << " bytes\n";
    }

    wxDateTime dt;
    wxDateTime atime, ctime;
    if (fn.GetTimes(&dt, &atime, &ctime))
    {
        info << "Created:  " << ctime.FormatISOCombined(' ') << "\n";
        info << "Modified: " << dt.FormatISOCombined(' ') << "\n";
        info << "Accessed: " << atime.FormatISOCombined(' ') << "\n";
    }

    wxTextCtrl* text =
        new wxTextCtrl(&dlg, wxID_ANY, info, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    mainSizer->Add(text, 1, wxEXPAND | wxALL, 10);

    wxButton* ok = new wxButton(&dlg, wxID_OK, "OK");
    mainSizer->Add(ok, 0, wxALIGN_CENTER | wxALL, 10);

    dlg.SetSizer(mainSizer);
    dlg.Fit();
    dlg.SetSize(wxSize(dlg.GetSize().GetWidth() + 300, dlg.GetSize().GetHeight() + 200));
    dlg.Centre();

    dlg.ShowModal();
}

wxBitmap EnsureSize(const wxBitmap& bmp, int w, int h)
{
    if (bmp.GetWidth() == w && bmp.GetHeight() == h)
        return bmp;  // już OK

    wxImage img = bmp.ConvertToImage();
    img         = img.Scale(w, h, wxIMAGE_QUALITY_HIGH);
    return wxBitmap(img);
}

wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
    return EnsureSize(wxArtProvider::GetBitmap(id, client, size), size.x, size.y);
}

wxArrayString GetSortedDirContents(const wxString& folderPath, wxDirFlags flag)
{
    wxArrayString result;
    wxDir dir(folderPath);
    if (!dir.IsOpened())
        return result;

    wxString name;
    bool cont = dir.GetFirst(&name, wxEmptyString, flag);
    while (cont)
    {
        result.Add(name);
        cont = dir.GetNext(&name);
    }

    // sortowanie alfabetyczne
    result.Sort();

    return result;
}

}  // namespace

ProjectPanel::ProjectPanel(wxWindow* parent, const wxString& rootPath, FileSelectedCallback fileSelectedCallback)
    : wxPanel(parent)
    , fileSelectedCallback(fileSelectedCallback)
    , rootFolder(rootPath)
{
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    projectTree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(220, -1), wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);

    // Ikony dla drzewa (16x16)
    treeImageList       = new wxImageList(16, 16, true);
    treeFolderClosedIdx = treeImageList->Add(CreateBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16)));
    treeFolderOpenIdx   = treeImageList->Add(CreateBitmap(wxART_FOLDER_OPEN, wxART_OTHER, wxSize(16, 16)));
    projectTree->AssignImageList(treeImageList);

    sizer->Add(projectTree, 1, wxEXPAND | wxALL, 2);
    CreateFilePanel(sizer);
    SetSizerAndFit(sizer);

    BuildTree();
    InitFileList();

    projectTree->Bind(wxEVT_TREE_SEL_CHANGED, &ProjectPanel::OnTreeSelChanged, this);

    projectTree->Bind(wxEVT_RIGHT_DOWN,
                      [this](wxMouseEvent& event)
                      {
                          int flags;
                          wxTreeItemId item = projectTree->HitTest(event.GetPosition(), flags);

                          if (item.IsOk())
                          {
                              projectTree->SelectItem(item);  // 👈 teraz PPM też zaznacza
                              auto* data = static_cast<PathData*>(projectTree->GetItemData(item));
                              if (data)
                              {
                                  contextMenuTriggerAction(event, projectTree, data->path);
                              }
                          }
                          else
                          {
                              UnSelectCurrentItem();
                          }
                      });

    RefreshCurrent(rootFolder);
}

void ProjectPanel::BuildTree()
{
    projectTree->DeleteAllItems();

    // Root jako widoczny element (unikamy wxTR_HIDE_ROOT i asercji)
    wxTreeItemId rootId = projectTree->AddRoot(GameEngine::File(rootFolder.ToStdString()).GetBaseName(), treeFolderClosedIdx,
                                               treeFolderOpenIdx, new PathData(rootFolder));

    AddSubDirs(rootId, rootFolder);
    projectTree->Expand(rootId);
}

void ProjectPanel::AddSubDirs(const wxTreeItemId& parentId, const wxString& absPath)
{
    wxDir dir(absPath);
    if (!dir.IsOpened())
        return;

    wxString name;
    bool cont = dir.GetFirst(&name, wxEmptyString, wxDIR_DIRS);
    while (cont)
    {
        wxFileName childPath(absPath, name);  // absPath/name
        wxString full = childPath.GetFullPath();

        wxTreeItemId childId =
            projectTree->AppendItem(parentId, name, treeFolderClosedIdx, treeFolderOpenIdx, new PathData(full));

        // Rekurencja — dociągamy kolejne poziomy
        AddSubDirs(childId, full);

        cont = dir.GetNext(&name);
    }
}

void ProjectPanel::InitFileList()
{
    // Lista ikon 64x64 dla widoku ikon
    fileImageList = new wxImageList(64, 64, true);
    // projectFiles->AssignImageList(fileImageList, wxIMAGE_LIST_NORMAL);

    // Bazowe pozycje zawsze na 0/1 — przy każdym odświeżeniu listy czyścimy tę listę,
    // więc indeksy są stabilne
    fileImageList->RemoveAll();
    idxFolder      = fileImageList->Add(CreateBitmap(wxART_FOLDER, wxART_OTHER, wxSize(64, 64)));
    idxDefaultFile = fileImageList->Add(CreateBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(64, 64)));
}

void ProjectPanel::SelectItem(wxPanel* itemPanel)
{
    if (selectedItemPanel)
    {
        // Odznacz poprzedni element
        selectedItemPanel->SetBackgroundColour(wxNullColour);
        selectedItemPanel->Refresh();
    }

    selectedItemPanel = itemPanel;

    if (selectedItemPanel)
    {
        // Zaznacz nowy element
        selectedItemPanel->SetBackgroundColour(*wxLIGHT_GREY);
        selectedItemPanel->Refresh();
    }
}

wxBoxSizer* ProjectPanel::CreateFileItem(const wxFileName& fn, const wxBitmap& bmp, bool isFolder,
                                         const std::function<void()>& onDClick)
{
    auto EllipsizeString = [&](const wxString& text, const wxFont& font, int maxWidth) -> wxString
    {
        wxMemoryDC dc;
        dc.SetFont(font);
        if (dc.GetTextExtent(text).GetWidth() <= maxWidth)
            return text;

        wxString ellipsis = "...";
        int ellipsisWidth = dc.GetTextExtent(ellipsis).GetWidth();

        wxString result;
        for (size_t i = 0; i < text.Length(); ++i)
        {
            wxString part = text.Left(i + 1);
            if (dc.GetTextExtent(part).GetWidth() + ellipsisWidth > maxWidth)
            {
                result = text.Left(i) + ellipsis;
                break;
            }
        }
        return result;
    };

    const int thumbSize = 64;

    wxStaticBitmap* icon = new wxStaticBitmap(filePanel, wxID_ANY, bmp);

    wxString shortName = EllipsizeString(fn.GetFullName(), GetFont(), thumbSize);
    wxStaticText* label =
        new wxStaticText(filePanel, wxID_ANY, shortName, wxDefaultPosition, wxSize(thumbSize, -1), wxALIGN_CENTRE_HORIZONTAL);

    icon->SetToolTip(fn.GetFullName());
    label->SetToolTip(fn.GetFullName());

    wxBoxSizer* itemSizer = new wxBoxSizer(wxVERTICAL);
    itemSizer->Add(icon, 0, wxALIGN_CENTER_HORIZONTAL);
    itemSizer->Add(label, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 2);

    // Kliknięcie = zaznaczenie
    label->Bind(wxEVT_LEFT_DOWN, [=](wxMouseEvent&) { SelectItem(icon, label); });
    icon->Bind(wxEVT_LEFT_DOWN,
               [=](wxMouseEvent& event)
               {
                   SelectItem(icon, label);
                   dragStartPos = event.GetPosition();
                   dragIcon     = icon;
               });

    icon->Bind(wxEVT_MOTION,
               [=](wxMouseEvent& event)
               {
                   if (event.Dragging() && event.LeftIsDown() && dragIcon)
                   {
                       wxFileDataObject fileData;
                       fileData.AddFile(fn.GetFullPath());
                       wxDropSource dragSource(dragIcon);
                       dragSource.SetData(fileData);
                       dragSource.DoDragDrop(wxDrag_CopyOnly);
                       dragIcon = nullptr;
                   }
               });

    icon->Bind(wxEVT_LEFT_UP, [=](wxMouseEvent&) { dragIcon = nullptr; });

    // Podwójny klik – różne akcje dla folderu i pliku
    if (onDClick)
    {
        icon->Bind(wxEVT_LEFT_DCLICK, [=](wxMouseEvent&) { onDClick(); });
    }

    addContextMenu(icon, FileInfo{.name = fn, .icon = icon, .label = label});
    addContextMenu(label, FileInfo{.name = fn, .icon = icon, .label = label});

    return itemSizer;
}

void ProjectPanel::SelectItem(wxWindow* item, wxStaticText* label)
{
    UnSelectCurrentItem();

    selectedItem = item;
    selectedItem->SetBackgroundColour(*wxLIGHT_GREY);
    selectedItem->Refresh();

    selectedLabel = label;
    selectedLabel->SetBackgroundColour(*wxLIGHT_GREY);
    selectedLabel->Refresh();
}

void ProjectPanel::UnSelectCurrentItem()
{
    if (selectedItem)
    {
        selectedItem->SetBackgroundColour(wxNullColour);
        selectedItem->Refresh();
    }

    if (selectedLabel)
    {
        selectedLabel->SetBackgroundColour(wxNullColour);
        selectedLabel->Refresh();
    }

    selectedItem  = nullptr;
    selectedLabel = nullptr;
}

void ProjectPanel::addContextMenu(wxWindow* target)
{
    target->Bind(wxEVT_RIGHT_DOWN,
                 [this, t = target](wxMouseEvent& event)
                 {
                     UnSelectCurrentItem();
                     contextMenuTriggerAction(event, t, GetCurrentFolderPath(), false);
                 });
}

void ProjectPanel::addContextMenu(wxWindow* target, const FileInfo& fileInfo)
{
    target->Bind(wxEVT_RIGHT_DOWN,
                 [=](wxMouseEvent& event)
                 {
                     SelectItem(fileInfo.icon, fileInfo.label);
                     contextMenuTriggerAction(event, target, fileInfo.name);
                 });
};

void ProjectPanel::contextMenuTriggerAction(wxMouseEvent& event, wxWindow* target, const wxFileName& fileName, bool removeAble)
{
    wxMenu menu;

    int ID_OPEN            = wxWindow::NewControlId();
    int ID_SHOW            = wxWindow::NewControlId();
    int ID_COPY_PATH       = wxWindow::NewControlId();
    int ID_DUPLICATE       = wxWindow::NewControlId();
    int ID_IMPORT          = wxWindow::NewControlId();
    int ID_IMPORT_FOLDER   = wxWindow::NewControlId();
    int ID_PASTE           = wxWindow::NewControlId();
    int ID_ANIMATION_VIWER = wxWindow::NewControlId();
    int ID_NEW_FOLDER      = wxWindow::NewControlId();
    int ID_REFRESH_FOLDER  = wxWindow::NewControlId();
    int ID_REMOVE          = wxWindow::NewControlId();
    int ID_PROPERTIES      = wxWindow::NewControlId();

    menu.Append(ID_OPEN, "Open");
    menu.Append(ID_SHOW, "Show in Explorer");
    menu.Append(ID_ANIMATION_VIWER, "Show in animation viewer");
    menu.AppendSeparator();
    menu.Append(ID_COPY_PATH, "Copy Path");
    menu.AppendSeparator();
    menu.Append(ID_DUPLICATE, "Duplicate");
    menu.Append(ID_IMPORT, "Import");
    menu.Append(ID_IMPORT_FOLDER, "Import folder");
    menu.Append(ID_PASTE, "Paste");
    menu.Append(ID_NEW_FOLDER, "New folder");
    menu.Append(ID_REMOVE, "Remove");
    menu.Append(ID_REFRESH_FOLDER, "Refresh folder");
    menu.AppendSeparator();
    menu.Append(ID_PROPERTIES, "Properties");

    GameEngine::File file(fileName.GetFullPath().ToStdString());
    menu.Enable(ID_ANIMATION_VIWER, is3dModelFile(file) or isPrefab(file));

    auto isCurrentDir = fileName == currentFolderPath;
    auto isRootItem   = fileName == rootFolder;

    menu.Enable(ID_OPEN, not isCurrentDir);
    menu.Enable(ID_DUPLICATE, not isCurrentDir);
    menu.Enable(ID_REMOVE, removeAble and not isRootItem);

    bool canPaste = false;
    if (wxTheClipboard->Open())
    {
        wxFileDataObject fileData;
        canPaste = wxTheClipboard->IsSupported(wxDF_FILENAME);
        wxTheClipboard->Close();
    }
    menu.Enable(ID_PASTE, canPaste);

    // --- Handlery dla menu ---
    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { wxLaunchDefaultApplication(fileName.GetFullPath()); }, ID_OPEN);

    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent&)
        {
            wxString folder = fileName.GetPath();
            wxString full   = fileName.GetFullPath();

#if defined(__WXMSW__)
            // Windows - zaznacz plik w Explorerze
            wxExecute("explorer.exe /select,\"" + full + "\"");

#elif defined(__WXGTK__)
            // Linux - spróbuj kilku menedżerów plików, fallback do xdg-open
            auto IsCommandAvailable = [](const wxString& cmd) -> bool
            { return wxExecute("command -v " + cmd, wxEXEC_SYNC) == 0; };

            if (IsCommandAvailable("nautilus"))
            {
                wxExecute("nautilus --select \"" + full + "\"", wxEXEC_ASYNC);
            }
            else if (IsCommandAvailable("nemo"))
            {
                wxExecute("nemo \"" + folder + "\"", wxEXEC_ASYNC);
            }
            else if (IsCommandAvailable("thunar"))
            {
                wxExecute("thunar \"" + folder + "\"", wxEXEC_ASYNC);
            }
            else
            {
                wxExecute("xdg-open \"" + folder + "\"", wxEXEC_ASYNC);
            }

#elif defined(__WXOSX__)
            // macOS - zaznacz plik w Finderze
            wxExecute("open -R \"" + full + "\"");
#endif
        },
        ID_SHOW);

    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent&) { runAnimationViewer("--file " + fileName.GetFullPath().ToStdString()); }, ID_ANIMATION_VIWER);
    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
            {
                wxTheClipboard->SetData(new wxTextDataObject(fileName.GetFullPath()));
                wxTheClipboard->Close();
            }
        },
        ID_COPY_PATH);

    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent&)
        {
            wxString newName = fileName.GetPathWithSep() + fileName.GetName() + "_copy." + fileName.GetExt();
            if (wxCopyFile(fileName.GetFullPath(), newName))
            {
                wxLogMessage("Duplicated to %s", newName);
                RefreshCurrent(currentFolderPath);
            }
            else
            {
                wxLogError("Failed to duplicate %s", fileName.GetFullPath());
            }
        },
        ID_DUPLICATE);

    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent&)
        {
            wxString destFolder = GetCurrentFolderPath();  // zakładam, że masz taką funkcję w ProjectPanel

            wxFileDialog fileDialog(this, "Select files to import", "", "", "All files (*.*)|*.*",
                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

            if (fileDialog.ShowModal() == wxID_OK)
            {
                wxArrayString files;
                fileDialog.GetPaths(files);

                for (auto& srcPath : files)
                {
                    wxFileName src(srcPath);
                    wxFileName dst(destFolder, src.GetFullName());

                    // Obsługa konfliktów nazw
                    if (wxFileExists(dst.GetFullPath()))
                    {
                        wxString base = dst.GetName();
                        wxString ext  = dst.GetExt();
                        int counter   = 1;
                        do
                        {
                            dst.SetName(base + "_" + std::to_string(counter));
                            dst.SetExt(ext);
                            counter++;
                        } while (wxFileExists(dst.GetFullPath()));
                    }

                    if (!wxCopyFile(src.GetFullPath(), dst.GetFullPath()))
                    {
                        wxLogError("Copy failed. From %s to %s", src.GetFullPath(), dst.GetFullPath());
                    }
                }

                // Odśwież listę po dodaniu plików
                RefreshCurrent(destFolder);
            }
        },
        ID_IMPORT);

    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent&)
        {
            wxString destFolder = GetCurrentFolderPath();

            wxDirDialog dirDialog(this, "Select folder to import", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
            if (dirDialog.ShowModal() != wxID_OK)
                return;

            std::filesystem::path srcFolder = dirDialog.GetPath().ToStdString();
            std::filesystem::path dstFolder = destFolder.ToStdString();

            Utils::CopyFileOrFolder(srcFolder, dstFolder);

            // Odśwież listę plików
            RefreshAll(destFolder);
        },
        ID_IMPORT_FOLDER);

    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent&)
        {
            wxTextEntryDialog nameDlg(this, "Enter folder name:");
            if (nameDlg.ShowModal() != wxID_OK)
                return;

            std::string name = nameDlg.GetValue().ToStdString();
            if (name.empty())
            {
                wxMessageBox("folder name cannot be empty!");
                return;
            }
            std::string parent;
            // if (fileName.IsDir())
            if (std::filesystem::is_directory(fileName.GetFullPath().ToStdString()))
            {
                parent = fileName.GetFullPath().ToStdString();
                wxMessageBox(parent);
            }
            else
            {
                parent = std::filesystem::path(fileName.GetFullPath().ToStdString()).parent_path().string();
                wxMessageBox(parent + "_getParent");
            }

            std::filesystem::create_directory(parent + "/" + name);
            RefreshAll(currentFolderPath);
        },
        ID_NEW_FOLDER);
    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { RefreshAll(currentFolderPath); }, ID_REFRESH_FOLDER);

    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent& evt)
        {
            bool isDirCopy{false};

            if (wxTheClipboard->Open())
            {
                wxFileDataObject fileData;
                if (wxTheClipboard->GetData(fileData))
                {
                    wxArrayString files = fileData.GetFilenames();
                    for (auto& f : files)
                    {
                        wxString targetFolder;
                        if (std::filesystem::is_directory(fileName.GetFullPath().ToStdString()))
                            targetFolder = fileName.GetFullPath();
                        else
                            targetFolder = fileName.GetPath();

                        isDirCopy = std::filesystem::is_directory(f.ToStdString());
                        Utils::CopyFileOrFolder(f.ToStdString(), targetFolder.ToStdString());
                    }
                }
                wxTheClipboard->Close();

                // Odśwież panel i drzewo
                if (isDirCopy)
                    RefreshAll(currentFolderPath);
                else
                    RefreshCurrent(currentFolderPath);
            }
        },
        ID_PASTE);

    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [=](wxCommandEvent&)
        {
            if (wxMessageBox("Remove " + fileName.GetFullPath() + "?", "Confirm", wxYES_NO | wxICON_WARNING) == wxYES)
            {
                auto isCurrentDir    = fileName == currentFolderPath;
                auto pathAfterRemove = isCurrentDir ? GetParentPath(currentFolderPath) : currentFolderPath;
                auto path            = fileName.GetFullPath().ToStdString();
                try
                {
                    if (std::filesystem::is_directory(path))
                    {
                        if (std::filesystem::is_empty(path))
                        {
                            std::filesystem::remove(path);
                            RefreshAll(pathAfterRemove);
                        }
                        else
                        {
                            if (wxMessageBox("Folder is not empty. Remove everything?", "Confirm", wxYES_NO | wxICON_WARNING) ==
                                wxYES)
                            {
                                std::filesystem::remove_all(path);
                                RefreshAll(pathAfterRemove);
                            }
                        }
                    }
                    else if (wxRemoveFile(fileName.GetFullPath()))
                    {
                        RefreshCurrent(pathAfterRemove);
                    }
                    else
                    {
                        wxLogError("Failed to remove %s", fileName.GetFullPath());
                    }
                }
                catch (const std::filesystem::filesystem_error& e)
                {
                    wxLogError("Filesystem error: %s", e.what());
                }
            }
        },
        ID_REMOVE);

    target->Bind(
        wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent& evt) { ShowProperties(fileName); }, ID_PROPERTIES);

    // Popup (ważne: użyj lokalnych współrzędnych → na globalne)
    target->PopupMenu(&menu, event.GetPosition());
}

void ProjectPanel::RefreshCurrent(const wxString& folderPath)
{
    currentFolderPath = folderPath;
    fileSizer->Clear(true);
    selectedItem  = nullptr;
    selectedLabel = nullptr;
    filePanel->Refresh();

    wxDir dir(folderPath);
    if (!dir.IsOpened())
        return;

    const int thumbSize = 64;

    // --- Foldery ---
    auto dirs = GetSortedDirContents(folderPath, wxDIR_DIRS);
    for (const wxString& name : dirs)
    {
        wxFileName fn(folderPath, name);
        wxBitmap bmp   = CreateBitmap(wxART_FOLDER, wxART_OTHER, wxSize(thumbSize, thumbSize));
        auto itemSizer = CreateFileItem(fn, bmp, true, [=]() { SelectTreeItemByPath(fn.GetFullPath()); });
        fileSizer->Add(itemSizer, 0, wxALL, 5);
    }

    // --- Pliki ---
    auto files = GetSortedDirContents(folderPath, wxDIR_FILES);
    for (const wxString& name : files)
    {
        wxFileName fn(folderPath, name);
        wxBitmap bmp   = GetThumbnail(fn, thumbSize);
        auto itemSizer = CreateFileItem(fn, bmp, false, [=]() { fileSelectedCallback(fn.GetFullPath()); });
        fileSizer->Add(itemSizer, 0, wxALL, 5);
    }

    fileSizer->Layout();
    filePanel->FitInside();
    filePanel->Refresh();
}

void ProjectPanel::RefreshAll()
{
    RefreshAll(currentFolderPath);
}

void ProjectPanel::RefreshAll(const wxString& folderPath)
{
    BuildTree();
    RefreshCurrent(folderPath);
    SelectTreeItemByPath(folderPath);
}

void ProjectPanel::OnTreeSelChanged(wxTreeEvent& e)
{
    wxTreeItemId id = e.GetItem();
    if (!id.IsOk())
        return;

    auto* data                = static_cast<PathData*>(projectTree->GetItemData(id));
    const wxString folderPath = data ? data->path : rootFolder;

    RefreshCurrent(folderPath.ToStdString());
}

void ProjectPanel::SelectTreeItemByPath(const wxString& path)
{
    wxTreeItemId rootId = projectTree->GetRootItem();
    if (not rootId.IsOk())
        return;

    wxTreeItemId found = FindTreeItemByPath(rootId, path);
    if (found.IsOk())
    {
        projectTree->SelectItem(found);
        projectTree->Expand(found);
        projectTree->EnsureVisible(found);
    }
}

wxTreeItemId ProjectPanel::FindTreeItemByPath(wxTreeItemId parent, const wxString& path)
{
    if (not parent.IsOk())
        return wxTreeItemId();

    auto* data = static_cast<PathData*>(projectTree->GetItemData(parent));
    if (data && data->path == path)
        return parent;

    wxTreeItemIdValue cookie;
    wxTreeItemId child = projectTree->GetFirstChild(parent, cookie);
    while (child.IsOk())
    {
        wxTreeItemId result = FindTreeItemByPath(child, path);
        if (result.IsOk())
            return result;
        child = projectTree->GetNextChild(parent, cookie);
    }
    return wxTreeItemId();
}

void ProjectPanel::CreateFilePanel(wxBoxSizer* mainSizer)
{
    filePanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);
    fileSizer = new wxWrapSizer(wxHORIZONTAL, wxWRAPSIZER_DEFAULT_FLAGS);
    filePanel->SetSizer(fileSizer);
    filePanel->SetScrollRate(10, 10);

    filePanel->SetDropTarget(new FileDropTarget(this));

    addContextMenu(filePanel);

    mainSizer->Add(filePanel, 3, wxEXPAND | wxALL, 2);
}

wxBitmap ProjectPanel::GetThumbnail(const wxFileName& fn, int thumbSize)
{
    wxString key = fn.GetFullPath();

    // Sprawdzenie cache globalnego
    wxBitmap bmp = ThumbnailCache::Get().GetThumbnail(key, thumbSize);

    // Jeśli bitmapa jest pusta (nie obraz), stwórz placeholder
    if (!bmp.IsOk())
    {
        if (is3dModelFile(fn.GetFullPath().ToStdString()))
        {
            auto newBmp = wxBitmap::NewFromPNGData(model3d_icon_png, model3d_icon_png_len);
            wxImage img = newBmp.ConvertToImage();
            img.Rescale(thumbSize, thumbSize, wxIMAGE_QUALITY_HIGH);
            bmp = img;
        }
        else
        {
            bmp = CreateBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(thumbSize, thumbSize));
        }
    }

    return bmp;
}

wxString ProjectPanel::GetCurrentFolderPath() const
{
    return currentFolderPath;
}
