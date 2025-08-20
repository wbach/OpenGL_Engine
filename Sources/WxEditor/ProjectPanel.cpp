#include "ProjectPanel.h"

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
    projectFiles = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                  wxLC_ICON | wxLC_SINGLE_SEL | wxLC_AUTOARRANGE | wxLC_ALIGN_TOP);

    sizer->Add(projectTree, 1, wxEXPAND | wxALL, 2);
    sizer->Add(projectFiles, 3, wxEXPAND | wxALL, 2);
    SetSizerAndFit(sizer);

    // Inicjalizacja paneli
    BuildTree();
    InitFileList();

    // Bindowanie eventów (instancja-specyficzne, pewniejsze niż tablica zdarzeń)
    projectTree->Bind(wxEVT_TREE_SEL_CHANGED, &ProjectPanel::OnTreeSelChanged, this);
    projectFiles->Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectPanel::OnFileActivated, this);

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
    projectFiles->AssignImageList(fileImageList, wxIMAGE_LIST_NORMAL);

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
    // Czyścimy wpisy i miniatury (aby nie puchły indeksy ikon)
    projectFiles->DeleteAllItems();
    fileImageList->RemoveAll();
    idxFolder      = fileImageList->Add(CreateBitmap(wxART_FOLDER, wxART_OTHER, wxSize(64, 64)));
    idxDefaultFile = fileImageList->Add(CreateBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(64, 64)));

    wxDir dir(folderPath);
    if (!dir.IsOpened())
        return;

    // 1) Najpierw foldery
    {
        wxString name;
        bool cont = dir.GetFirst(&name, wxEmptyString, wxDIR_DIRS);
        while (cont)
        {
            long item = projectFiles->InsertItem(projectFiles->GetItemCount(), name, idxFolder);
            projectFiles->SetItemData(item, item);
            cont = dir.GetNext(&name);
        }
    }

    // 2) Potem pliki
    {
        wxString name;
        bool cont = dir.GetFirst(&name, wxEmptyString, wxDIR_FILES);
        while (cont)
        {
            wxFileName fn(folderPath, name);
            wxString ext = fn.GetExt().Lower();

            int imgIdx = idxDefaultFile;

            if (IsImageExt(ext))
            {
                wxLogNull noLog;
                wxImage img(fn.GetFullPath(), wxBITMAP_TYPE_ANY);
                if (img.IsOk())
                {
                    // Zachowanie proporcji przy skalowaniu do miniatury 64x64
                    int w = img.GetWidth();
                    int h = img.GetHeight();
                    if (w > 0 && h > 0)
                    {
                        const int thumb = 64;
                        double scale    = std::min(double(thumb) / w, double(thumb) / h);
                        int nw          = std::max(1, int(w * scale));
                        int nh          = std::max(1, int(h * scale));
                        img             = img.Scale(nw, nh, wxIMAGE_QUALITY_HIGH);
                    }
                    img.SetOption(wxIMAGE_OPTION_QUALITY, 100);
                    if (!img.HasAlpha())
                        img.InitAlpha();

                    imgIdx = fileImageList->Add(EnsureSize(wxBitmap(img), 64, 64));
                }
            }

            long item = projectFiles->InsertItem(projectFiles->GetItemCount(), name, imgIdx);
            projectFiles->SetItemData(item, item);

            cont = dir.GetNext(&name);
        }
    }

    // Upewnij się, że layout został wyliczony
    Layout();
    Refresh();
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

    wxString name = projectFiles->GetItemText(e.GetIndex());
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
