#include "ProjectPanel.h"

#include <wx/renderer.h>

namespace
{
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

}  // namespace

ProjectPanel::ProjectPanel(wxWindow* parent, const wxString& rootPath)
    : wxPanel(parent)
    , rootFolder(rootPath)
{
    // Handlery obrazów (dla PNG/JPG itp.)
    wxInitAllImageHandlers();

    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    // --- Drzewo folderów (po lewej) ---
    projectTree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(220, -1), wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);

    // Ikony dla drzewa (16x16)
    treeImageList       = new wxImageList(16, 16, true);
    treeFolderClosedIdx = treeImageList->Add(CreateBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16)));
    treeFolderOpenIdx   = treeImageList->Add(CreateBitmap(wxART_FOLDER_OPEN, wxART_OTHER, wxSize(16, 16)));
    projectTree->AssignImageList(treeImageList);

    // --- Lista plików (po prawej) ---
    //    projectFiles = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    //                                  wxLC_ICON | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE | wxLC_ALIGN_TOP);

    sizer->Add(projectTree, 1, wxEXPAND | wxALL, 2);
    CreateFilePanel(sizer);
    // sizer->Add(projectFiles, 3, wxEXPAND | wxALL, 2);
    SetSizerAndFit(sizer);

    // Inicjalizacja paneli
    BuildTree();
    InitFileList();

    // Bindowanie eventów (instancja-specyficzne, pewniejsze niż tablica zdarzeń)
    projectTree->Bind(wxEVT_TREE_SEL_CHANGED, &ProjectPanel::OnTreeSelChanged, this);
    // projectFiles->Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectPanel::OnFileActivated, this);

    // Start: pokaż zawartość root
    RefreshListFor(rootFolder);
}

void ProjectPanel::BuildTree()
{
    projectTree->DeleteAllItems();

    // Root jako widoczny element (unikamy wxTR_HIDE_ROOT i asercji)
    wxTreeItemId rootId = projectTree->AddRoot(rootFolder,  // tekst (może być pełna ścieżka lub nazwa projektu)
                                               treeFolderClosedIdx, treeFolderOpenIdx,
                                               new PathData(rootFolder)  // zapisujemy pełną ścieżkę
    );

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

static bool IsImageExt(const wxString& extLower)
{
    // Rozszerzaj wg potrzeb
    return extLower == "png" || extLower == "jpg" || extLower == "jpeg" || extLower == "bmp" || extLower == "gif" ||
           extLower == "tif" || extLower == "tiff";
}

void ProjectPanel::RefreshListFor(const wxString& folderPath)
{
    // Czyścimy panel i sizer
    fileSizer->Clear(true);  // usuwa dzieci i niszczy kontrolki
    filePanel->Refresh();

    wxDir dir(folderPath);
    if (!dir.IsOpened())
        return;

    wxString name;
    bool cont;

    const int thumbSize = 64;

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

    // --- Najpierw foldery ---
    cont = dir.GetFirst(&name, wxEmptyString, wxDIR_DIRS);
    while (cont)
    {
        wxFileName fn(folderPath, name);

        // Ikona folderu
        wxBitmap bmp         = CreateBitmap(wxART_FOLDER, wxART_OTHER, wxSize(thumbSize, thumbSize));
        wxStaticBitmap* icon = new wxStaticBitmap(filePanel, wxID_ANY, bmp);

        // Etykieta z przyciętym tekstem
        wxString shortName = EllipsizeString(name, GetFont(), thumbSize);
        wxStaticText* label =
            new wxStaticText(filePanel, wxID_ANY, shortName, wxDefaultPosition, wxSize(thumbSize, -1), wxALIGN_CENTRE_HORIZONTAL);

        // Kontener pionowy
        wxBoxSizer* itemSizer = new wxBoxSizer(wxVERTICAL);
        itemSizer->Add(icon, 0, wxALIGN_CENTER_HORIZONTAL);
        itemSizer->Add(label, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 2);
        icon->SetToolTip(name);
        label->SetToolTip(name);
        fileSizer->Add(itemSizer, 0, wxALL, 5);

        // Podwójny klik = zmiana folderu w drzewie
        icon->Bind(wxEVT_LEFT_DCLICK,
                   [=](wxMouseEvent&)
                   {
                       // Znajdź element w drzewie
                       wxTreeItemIdValue cookie;
                       wxTreeItemId root = projectTree->GetRootItem();
                       wxTreeItemId item;
                       for (item = projectTree->GetFirstChild(root, cookie); item.IsOk();
                            item = projectTree->GetNextChild(root, cookie))
                       {
                           auto* data = static_cast<PathData*>(projectTree->GetItemData(item));
                           if (data && data->path == fn.GetFullPath())
                           {
                               projectTree->SelectItem(item);
                               return;
                           }
                       }
                       // Jeśli nie znaleziono, po prostu odśwież listę
                       RefreshListFor(fn.GetFullPath());
                   });

        cont = dir.GetNext(&name);
    }

    // --- Potem pliki ---
    cont = dir.GetFirst(&name, wxEmptyString, wxDIR_FILES);
    while (cont)
    {
        wxFileName fn(folderPath, name);
        wxBitmap bmp = CreateBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(thumbSize, thumbSize));

        // Miniaturka dla obrazów
        wxString ext = fn.GetExt().Lower();
        if (IsImageExt(ext))
        {
            wxLogNull noLog;
            wxImage img(fn.GetFullPath(), wxBITMAP_TYPE_ANY);
            if (img.IsOk())
            {
                int w = img.GetWidth();
                int h = img.GetHeight();
                if (w > 0 && h > 0)
                {
                    double scale = std::min(double(thumbSize) / w, double(thumbSize) / h);
                    int nw       = std::max(1, int(w * scale));
                    int nh       = std::max(1, int(h * scale));
                    img          = img.Scale(nw, nh, wxIMAGE_QUALITY_HIGH);
                }
                img.SetOption(wxIMAGE_OPTION_QUALITY, 100);
                if (not img.HasAlpha())
                    img.InitAlpha();
                bmp = wxBitmap(img);
            }
        }

        wxStaticBitmap* icon = new wxStaticBitmap(filePanel, wxID_ANY, bmp);
        wxString shortName   = EllipsizeString(name, GetFont(), thumbSize);
        wxStaticText* label =
            new wxStaticText(filePanel, wxID_ANY, shortName, wxDefaultPosition, wxSize(thumbSize, -1), wxALIGN_CENTRE_HORIZONTAL);
        icon->SetToolTip(name);
        label->SetToolTip(name);
        wxBoxSizer* itemSizer = new wxBoxSizer(wxVERTICAL);
        itemSizer->Add(icon, 0, wxALIGN_CENTER_HORIZONTAL);
        itemSizer->Add(label, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 2);

        fileSizer->Add(itemSizer, 0, wxALL, 5);

        // Kliknięcie dwukrotne
        icon->Bind(wxEVT_LEFT_DCLICK, [=](wxMouseEvent&) { wxLogMessage("Wybrano: %s", fn.GetFullPath()); });

        cont = dir.GetNext(&name);
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

void ProjectPanel::OnFileActivated(wxListEvent& e)
{
    // Przykładowa obsługa dwukliku (zbudowanie pełnej ścieżki)
    wxTreeItemId sel = projectTree->GetSelection();
    if (!sel.IsOk())
        return;

    auto* data                = static_cast<PathData*>(projectTree->GetItemData(sel));
    const wxString folderPath = data ? data->path : rootFolder;

    wxString name;  //= projectFiles->GetItemText(e.GetIndex());
    wxFileName full(folderPath, name);

    if (full.DirExists())
    {
        // 🔹 Jeśli to folder → znajdź go w drzewie i zaznacz
        SelectTreeItemByPath(full.GetFullPath());
    }
    else
    {
        // 🔹 Jeśli to plik → tylko log (albo otwieranie edytora, co planujesz dalej)
        wxLogMessage("Wybrano plik: %s", full.GetFullPath());
    }
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

void ProjectPanel::OnThumbClicked(wxMouseEvent& e)
{
    wxPanel* thumb = dynamic_cast<wxPanel*>(e.GetEventObject());
    if (!thumb) return;

    wxStaticText* label = dynamic_cast<wxStaticText*>(thumb->GetChildren()[1]);
    if (!label) return;

    wxString name = label->GetLabel();

    // Wybrany folder?
    wxDir dir(rootFolder);
    wxString folderPath;
    wxTreeItemId sel = projectTree->GetSelection();
    if (sel.IsOk())
    {
        auto* data = static_cast<PathData*>(projectTree->GetItemData(sel));
        folderPath = data ? data->path : rootFolder;
    }
    else
        folderPath = rootFolder;

    wxFileName fn(folderPath, name);
    if (fn.DirExists())
    {
        // zaznacz w drzewie
        wxTreeItemIdValue cookie;
        wxTreeItemId child = projectTree->GetFirstChild(projectTree->GetSelection(), cookie);
        while (child.IsOk())
        {
            auto* pdata = static_cast<PathData*>(projectTree->GetItemData(child));
            if (pdata && pdata->path == fn.GetFullPath())
            {
                projectTree->SelectItem(child);
                break;
            }
            child = projectTree->GetNextChild(projectTree->GetSelection(), cookie);
        }
    }
    else
    {
        wxLogMessage("Kliknięto plik: %s", fn.GetFullPath());
    }
}
