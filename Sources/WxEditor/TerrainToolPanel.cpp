#include "TerrainToolPanel.h"

#include <wx/collpane.h>

// clang-format off
wxBEGIN_EVENT_TABLE(TerrainToolPanel, wxPanel)
    EVT_BUTTON(wxID_CLOSE, TerrainToolPanel::OnClose)
wxEND_EVENT_TABLE()

TerrainToolPanel::TerrainToolPanel(wxWindow* parent, int width)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(width, -1), wxBORDER_NONE | wxTAB_TRAVERSAL)
// clang-format on
{
    SetBackgroundColour(wxColour(30, 30, 30));
    SetTransparent(200);

    BuildUI();

    SetScrollRate(5, 5);

    Layout();
    FitInside();
}

void TerrainToolPanel::BuildUI()
{
    // Główny sizer panelu
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    auto* headerSizer = new wxBoxSizer(wxHORIZONTAL);

    // Tytuł po lewej
    wxStaticText* title = new wxStaticText(this, wxID_ANY, "Terrain Tools");
    headerSizer->Add(title, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);

    // Rozpycha przycisk w prawą stronę
    headerSizer->AddStretchSpacer();

    // Mały przycisk Close
    wxButton* closeBtn = new wxButton(this, wxID_CLOSE, "X", wxDefaultPosition, wxSize(40, 20));
    headerSizer->Add(closeBtn, 0, wxALL, 2);

    mainSizer->Add(headerSizer, 0, wxEXPAND);

    // Ramka generatora
    BuildTerrainGeneratorUI(mainSizer);
    BuildTerrainPainterUI(mainSizer);

    // Tutaj można później dodać kolejne sekcje, np. Texture Painter
    // BuildTexturePainterUI(mainSizer);

    SetSizerAndFit(mainSizer);

    Layout();
    FitInside();
}

void TerrainToolPanel::BuildTerrainGeneratorUI(wxSizer* parentSizer)
{
    // Tworzymy collapsible pane
    auto* collapsible =
        new wxCollapsiblePane(this, wxID_ANY, "Terrain Generator", wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE);

    // Panel wewnątrz collapsible
    wxWindow* pane  = collapsible->GetPane();
    auto* paneSizer = new wxBoxSizer(wxVERTICAL);

    // GameObjectId
    gameObjectIdCtrl = new wxTextCtrl(pane, wxID_ANY, "---");
    auto* goSizer    = new wxStaticBoxSizer(wxVERTICAL, pane, "GameObjectId");
    goSizer->Add(gameObjectIdCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(goSizer, 0, wxEXPAND | wxALL, 5);

    // Bias
    biasCtrl        = new wxTextCtrl(pane, wxID_ANY, "2.0");
    auto* biasSizer = new wxStaticBoxSizer(wxVERTICAL, pane, "Bias");
    biasSizer->Add(biasCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(biasSizer, 0, wxEXPAND | wxALL, 5);

    // Octaves
    octavesCtrl        = new wxTextCtrl(pane, wxID_ANY, "9");
    auto* octavesSizer = new wxStaticBoxSizer(wxVERTICAL, pane, "Octaves");
    octavesSizer->Add(octavesCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(octavesSizer, 0, wxEXPAND | wxALL, 5);

    // Width
    widthCtrl        = new wxTextCtrl(pane, wxID_ANY, "512");
    auto* widthSizer = new wxStaticBoxSizer(wxVERTICAL, pane, "Width");
    widthSizer->Add(widthCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(widthSizer, 0, wxEXPAND | wxALL, 5);

    // Buttons
    wxButton* btnSameSeed = new wxButton(pane, wxID_ANY, "Generate with the same seed");
    btnSameSeed->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {});
    paneSizer->Add(btnSameSeed, 0, wxEXPAND | wxALL, 5);

    wxButton* btnNewSeed = new wxButton(pane, wxID_ANY, "Generate terrain with new seed");
    btnNewSeed->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {});
    paneSizer->Add(btnNewSeed, 0, wxEXPAND | wxALL, 5);

    // Ustawiamy sizer wewnątrz collapsible pane
    pane->SetSizer(paneSizer);

    // Dodajemy collapsible pane do sizer-a głównego
    parentSizer->Add(collapsible, 0, wxEXPAND | wxALL, 5);

    // Opcjonalnie można rozpocząć z rozwiniętym pane
    collapsible->Collapse(false);

    // Aktualizacja layoutu po zmianie stanu collapsible
    collapsible->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED,
                      [this](wxCollapsiblePaneEvent&)
                      {
                          Layout();
                          FitInside();
                          if (GetParent())
                              GetParent()->Layout();
                      });
}

void TerrainToolPanel::BuildTerrainPainterUI(wxSizer* parentSizer)
{
    // Tworzymy collapsible pane
    auto* collapsible =
        new wxCollapsiblePane(this, wxID_ANY, "Terrain Painter", wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE);

    wxWindow* pane  = collapsible->GetPane();
    auto* paneSizer = new wxBoxSizer(wxVERTICAL);

    // Brush type (combobox)
    auto brushTypeCtrl = new wxComboBox(pane, wxID_ANY);
    auto* brushSizer   = new wxStaticBoxSizer(wxVERTICAL, pane, "Brush Type");
    brushSizer->Add(brushTypeCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(brushSizer, 0, wxEXPAND | wxALL, 5);

    // Step interpolation (combobox)
    auto stepInterpolationCtrl = new wxComboBox(pane, wxID_ANY);
    auto* stepSizer            = new wxStaticBoxSizer(wxVERTICAL, pane, "Step Interpolation");
    stepSizer->Add(stepInterpolationCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(stepSizer, 0, wxEXPAND | wxALL, 5);

    // Brush size (slider)
    auto brushSizeCtrl   = new wxSlider(pane, wxID_ANY, 25, 1, 50);
    auto* brushSizeSizer = new wxStaticBoxSizer(wxVERTICAL, pane, "Brush Size");
    brushSizeSizer->Add(brushSizeCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(brushSizeSizer, 0, wxEXPAND | wxALL, 5);

    // Strength (text input)
    auto strengthCtrl   = new wxTextCtrl(pane, wxID_ANY, "1.0");
    auto* strengthSizer = new wxStaticBoxSizer(wxVERTICAL, pane, "Strength");
    strengthSizer->Add(strengthCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(strengthSizer, 0, wxEXPAND | wxALL, 5);

    // Buttons (bez akcji)
    wxButton* btnGenerate = new wxButton(pane, wxID_ANY, "Generate");
    paneSizer->Add(btnGenerate, 0, wxEXPAND | wxALL, 5);

    wxButton* btnEraseMode = new wxButton(pane, wxID_ANY, "Erase Mode");
    paneSizer->Add(btnEraseMode, 0, wxEXPAND | wxALL, 5);

    wxButton* btnRecalculateNormals = new wxButton(pane, wxID_ANY, "Recalculate Normals");
    paneSizer->Add(btnRecalculateNormals, 0, wxEXPAND | wxALL, 5);

    // Ustawiamy sizer w collapsible pane
    pane->SetSizer(paneSizer);

    // Dodajemy collapsible pane do głównego sizer-a
    parentSizer->Add(collapsible, 0, wxEXPAND | wxALL, 5);

    // Opcjonalnie można rozpocząć z rozwiniętym pane
    collapsible->Collapse(false);

    // Aktualizacja layoutu po zmianie stanu collapsible
    collapsible->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED,
                      [this](wxCollapsiblePaneEvent&)
                      {
                          Layout();
                          FitInside();
                          if (GetParent())
                              GetParent()->Layout();
                      });
}

void TerrainToolPanel::ShowPanel(bool show)
{
    Show(show);
    isVisible = show;
}

void TerrainToolPanel::OnClose(wxCommandEvent& event)
{
    ShowPanel(false);
}
