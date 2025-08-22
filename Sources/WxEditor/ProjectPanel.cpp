#include "ProjectPanel.h"

#include <Logger/Log.h>
#include <wx/clipbrd.h>
#include <wx/dnd.h>
#include <wx/renderer.h>

#include "ThumbnailCache.h"

namespace
{
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
    RefreshListFor(rootFolder);
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

    auto SelectItem = [&](wxWindow* item, wxStaticText* label)
    {
        if (selectedItem)
        {
            selectedItem->SetBackgroundColour(wxNullColour);
            selectedItem->Refresh();
        }
        selectedItem = item;
        selectedItem->SetBackgroundColour(*wxLIGHT_GREY);
        selectedItem->Refresh();

        if (selectedLabel)
        {
            selectedLabel->SetBackgroundColour(wxNullColour);
            selectedLabel->Refresh();
        }

        selectedLabel = label;
        selectedLabel->SetBackgroundColour(*wxLIGHT_GREY);
        selectedLabel->Refresh();
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

    auto addContextMenu = [&](wxWindow* target, wxFileName fn)
    {
        target->Bind(
            wxEVT_RIGHT_DOWN,
            [=](wxMouseEvent& event)
            {
                SelectItem(icon, label);

                wxMenu menu;

                int ID_OPEN       = wxWindow::NewControlId();
                int ID_SHOW       = wxWindow::NewControlId();
                int ID_COPY_PATH  = wxWindow::NewControlId();
                int ID_DUPLICATE  = wxWindow::NewControlId();
                int ID_PASTE      = wxWindow::NewControlId();
                int ID_REMOVE     = wxWindow::NewControlId();
                int ID_PROPERTIES = wxWindow::NewControlId();

                menu.Append(ID_OPEN, "Open");
                menu.Append(ID_SHOW, "Show in Explorer");
                menu.AppendSeparator();
                menu.Append(ID_COPY_PATH, "Copy Path");
                menu.AppendSeparator();
                menu.Append(ID_DUPLICATE, "Duplicate");
                menu.Append(ID_PASTE, "Paste");
                menu.Append(ID_REMOVE, "Remove");
                menu.AppendSeparator();
                menu.Append(ID_PROPERTIES, "Properties");

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
                    wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { wxLaunchDefaultApplication(fn.GetFullPath()); }, ID_OPEN);

                menu.Bind(
                    wxEVT_COMMAND_MENU_SELECTED,
                    [=](wxCommandEvent&)
                    {
                        wxString folder = fn.GetPath();
#if defined(__WXMSW__)
                        wxExecute("explorer.exe /select,\"" + fn.GetFullPath() + "\"");
#elif defined(__WXGTK__)
                        wxString cmd = "nautilus --select \"" + fn.GetFullPath() + "\"";
                        if (wxExecute(cmd, wxEXEC_ASYNC) != 0)
                            wxExecute("xdg-open \"" + fn.GetPath() + "\"", wxEXEC_ASYNC);
#elif defined(__WXOSX__)
                        wxExecute("open -R \"" + fn.GetFullPath() + "\"");
#endif
                    },
                    ID_SHOW);

                menu.Bind(
                    wxEVT_COMMAND_MENU_SELECTED,
                    [=](wxCommandEvent&)
                    {
                        if (wxTheClipboard->Open())
                        {
                            wxTheClipboard->SetData(new wxTextDataObject(fn.GetFullPath()));
                            wxTheClipboard->Close();
                        }
                    },
                    ID_COPY_PATH);

                menu.Bind(
                    wxEVT_COMMAND_MENU_SELECTED,
                    [=](wxCommandEvent&)
                    {
                        wxString newName = fn.GetPathWithSep() + fn.GetName() + "_copy." + fn.GetExt();
                        if (wxCopyFile(fn.GetFullPath(), newName))
                        {
                            wxLogMessage("Duplicated to %s", newName);
                            RefreshListFor(currentFolderPath);
                        }
                        else
                        {
                            wxLogError("Failed to duplicate %s", fn.GetFullPath());
                        }
                    },
                    ID_DUPLICATE);

                menu.Bind(
                    wxEVT_COMMAND_MENU_SELECTED,
                    [=](wxCommandEvent& evt)
                    {
                        if (wxTheClipboard->Open())
                        {
                            wxFileDataObject fileData;
                            if (wxTheClipboard->GetData(fileData))
                            {
                                wxArrayString files = fileData.GetFilenames();
                                for (auto& f : files)
                                {
                                    wxString targetFolder;
                                    if (fn.IsDir())
                                        targetFolder = fn.GetFullPath();  // wklejamy do katalogu
                                    else
                                        targetFolder = fn.GetPath();  // wklejamy obok pliku

                                    wxFileName src(f);
                                    wxFileName dst(targetFolder, src.GetFullName());

                                    if (wxCopyFile(src.GetFullPath(), dst.GetFullPath()))
                                    {
                                        wxLogMessage("Copied %s to %s", src.GetFullPath(), dst.GetFullPath());
                                        RefreshListFor(currentFolderPath);  // odśwież listę plików
                                    }
                                    else
                                        wxLogError("Failed to copy %s", src.GetFullPath());
                                }
                            }
                            wxTheClipboard->Close();
                        }
                    },
                    ID_PASTE);

                menu.Bind(
                    wxEVT_COMMAND_MENU_SELECTED,
                    [=](wxCommandEvent&)
                    {
                        if (wxMessageBox("Remove " + fn.GetFullPath() + "?", "Confirm", wxYES_NO | wxICON_WARNING) == wxYES)
                        {
                            if (wxRemoveFile(fn.GetFullPath()))
                            {
                                wxLogMessage("Removed %s", fn.GetFullPath());
                                RefreshListFor(currentFolderPath);
                            }
                            else
                            {
                                wxLogError("Failed to remove %s", fn.GetFullPath());
                            }
                        }
                    },
                    ID_REMOVE);

                target->Bind(wxEVT_MENU,
                             [=](wxCommandEvent& evt)
                             {
                                 if (evt.GetId() == ID_PROPERTIES)
                                     ShowProperties(fn);
                             });

                // Popup (ważne: użyj lokalnych współrzędnych → na globalne)
                target->PopupMenu(&menu, event.GetPosition());
            });
    };

    addContextMenu(icon, fn);
    addContextMenu(label, fn);
    // addContextMenu(filePanel, currentFolderPath); multiple binds problem

    return itemSizer;
}

void ProjectPanel::RefreshListFor(const wxString& folderPath)
{
    currentFolderPath = folderPath;
    fileSizer->Clear(true);
    selectedItem  = nullptr;
    selectedLabel = nullptr;
    filePanel->Refresh();

    wxDir dir(folderPath);
    if (!dir.IsOpened())
        return;

    SelectTreeItemByPath(folderPath);

    const int thumbSize = 64;

    // --- Foldery ---
    auto dirs = GetSortedDirContents(folderPath, wxDIR_DIRS);
    for (const wxString& name : dirs)
    {
        wxFileName fn(folderPath, name);
        wxBitmap bmp   = CreateBitmap(wxART_FOLDER, wxART_OTHER, wxSize(thumbSize, thumbSize));
        auto itemSizer = CreateFileItem(fn, bmp, true, [=]() { RefreshListFor(fn.GetFullPath()); });
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
void ProjectPanel::OnTreeSelChanged(wxTreeEvent& e)
{
    wxTreeItemId id = e.GetItem();
    if (!id.IsOk())
        return;

    auto* data                = static_cast<PathData*>(projectTree->GetItemData(id));
    const wxString folderPath = data ? data->path : rootFolder;

    RefreshListFor(folderPath);
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
        bmp = CreateBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(thumbSize, thumbSize));
    }

    return bmp;
}
