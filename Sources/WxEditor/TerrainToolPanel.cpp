#include "TerrainToolPanel.h"

#include <GameEngine/Components/ComponentController.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h>
#include <GameEngine/DebugTools/Painter/TerrainHeightGenerator.h>
#include <Logger/Log.h>
#include <Utils/Variant.h>
#include <wx/busyinfo.h>
#include <wx/collpane.h>
#include <wx/combobox.h>
#include <wx/simplebook.h>
#include <wx/wrapsizer.h>

#include <GameEngine/Scene/Scene.hpp>
#include <optional>

#include "Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "EditorUitls.h"
#include "LoadingDialog.h"
#include "ProjectManager.h"
#include "TextureButton.h"
#include "Types.h"

namespace
{
class TerrainObjectClientData : public wxClientData
{
public:
    enum class Method
    {
        createNewTerrainObject,
        getAllTerrains
    };

    explicit TerrainObjectClientData(IdType value)
        : value(value)
    {
    }
    explicit TerrainObjectClientData(Method value)
        : value(value)
    {
    }
    std::variant<Method, IdType> GetValue() const
    {
        return value;
    }

private:
    std::variant<Method, IdType> value;
};

}  // namespace

// clang-format off
wxBEGIN_EVENT_TABLE(TerrainToolPanel, wxPanel)
    EVT_BUTTON(wxID_CLOSE, TerrainToolPanel::OnClose)
wxEND_EVENT_TABLE()

TerrainToolPanel::TerrainToolPanel(wxWindow* parent, GameEngine::Scene& scene, int width)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(width, -1), wxBORDER_NONE | wxTAB_TRAVERSAL)
    , scene{scene}
// clang-format on
{
    SetBackgroundColour(wxColour(30, 30, 30));
    SetTransparent(200);

    BuildUI();

    SetScrollRate(5, 5);

    Layout();
    FitInside();

    sceneEventSubId = scene.SubscribeForSceneEvent(
        [this](const auto& event)
        { std::visit(visitor{[this](const auto& e) { this->CallAfter([this, e] { ProcessEvent(e); }); }}, event); });
}

TerrainToolPanel::~TerrainToolPanel()
{
    if (sceneEventSubId)
    {
        scene.UnSubscribeForSceneEvent(*sceneEventSubId);
    }
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
    auto* collapsible =
        new wxCollapsiblePane(this, wxID_ANY, "Terrain Generator", wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE);

    wxWindow* pane  = collapsible->GetPane();
    auto* paneSizer = new wxBoxSizer(wxVERTICAL);

    // GameObjectId

    generatorFields.gameObjectIdCtrl = new wxChoice(pane, wxID_ANY);

    RefillTerrainObjectsCtrl();

    // generatorFields.gameObjectIdCtrl = new wxTextCtrl(pane, wxID_ANY, "---");
    auto* goSizer = new wxStaticBoxSizer(wxVERTICAL, pane, "GameObject");
    goSizer->Add(generatorFields.gameObjectIdCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(goSizer, 0, wxEXPAND | wxALL, 5);

    // Sekcja Bias / Octaves w jednej linii
    auto* noiseRowSizer = new wxStaticBoxSizer(wxHORIZONTAL, pane, "Noise Settings");

    // Bias
    noiseRowSizer->Add(new wxStaticText(pane, wxID_ANY, "Bias:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    generatorFields.biasCtrl = new wxTextCtrl(pane, wxID_ANY, "2.0", wxDefaultPosition, wxSize(50, -1));
    noiseRowSizer->Add(generatorFields.biasCtrl, 0, wxRIGHT, 10);

    // Octaves
    noiseRowSizer->Add(new wxStaticText(pane, wxID_ANY, "Octaves:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    generatorFields.octavesCtrl = new wxTextCtrl(pane, wxID_ANY, "9", wxDefaultPosition, wxSize(50, -1));
    noiseRowSizer->Add(generatorFields.octavesCtrl, 0, wxRIGHT, 5);

    paneSizer->Add(noiseRowSizer, 0, wxEXPAND | wxALL, 5);

    // Width w osobnej linii, jako ComboBox
    auto* widthSizer = new wxStaticBoxSizer(wxHORIZONTAL, pane, "Heightmap Resolution");
    widthSizer->Add(new wxStaticText(pane, wxID_ANY, "Width:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    wxArrayString widthChoices;
    widthChoices.Add("256");
    widthChoices.Add("512");
    widthChoices.Add("1024");
    widthChoices.Add("2048");
    widthChoices.Add("4096");

    generatorFields.widthCtrl =
        new wxComboBox(pane, wxID_ANY, "512", wxDefaultPosition, wxDefaultSize, widthChoices, wxCB_READONLY);
    widthSizer->Add(generatorFields.widthCtrl, 1, wxEXPAND | wxRIGHT, 5);

    paneSizer->Add(widthSizer, 0, wxEXPAND | wxALL, 5);

    // Buttons
    wxButton* btnSameSeed = new wxButton(pane, wxID_ANY, "Generate with the same seed");
    btnSameSeed->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { GenerateTerrain(false); });
    paneSizer->Add(btnSameSeed, 0, wxEXPAND | wxALL, 5);

    wxButton* btnNewSeed = new wxButton(pane, wxID_ANY, "Generate terrain with new seed");
    btnNewSeed->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { GenerateTerrain(true); });
    paneSizer->Add(btnNewSeed, 0, wxEXPAND | wxALL, 5);

    wxButton* btnImportFromMesh = new wxButton(pane, wxID_ANY, "Import from mesh");
    btnImportFromMesh->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { ImportFromMesh(); });
    paneSizer->Add(btnImportFromMesh, 0, wxEXPAND | wxALL, 5);

    pane->SetSizer(paneSizer);
    parentSizer->Add(collapsible, 0, wxEXPAND | wxALL, 5);

    collapsible->Collapse(false);

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
    // === Collapsible Pane ===
    auto* collapsible =
        new wxCollapsiblePane(this, wxID_ANY, "Terrain Painter", wxDefaultPosition, wxDefaultSize, wxCP_DEFAULT_STYLE);
    wxWindow* pane  = collapsible->GetPane();
    auto* paneSizer = new wxBoxSizer(wxVERTICAL);

    // === Painter Type wybór ===
    auto* painterTypeBox = new wxStaticBoxSizer(wxVERTICAL, pane, "Painter Type");
    wxArrayString painterTypes;
    painterTypes.Add("Height");
    painterTypes.Add("Texture");
    painterTypes.Add("Plant");

    auto* painterTypeCombo =
        new wxComboBox(pane, wxID_ANY, "Height", wxDefaultPosition, wxDefaultSize, painterTypes, wxCB_READONLY);
    painterTypeBox->Add(painterTypeCombo, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(painterTypeBox, 0, wxEXPAND | wxALL, 5);

    // === Dynamiczna sekcja z painterami ===
    auto* dynamicBook = new wxSimplebook(pane, wxID_ANY);
    dynamicBook->AddPage(BuildHeightPainterPanel(dynamicBook), "Height");
    dynamicBook->AddPage(BuildTexturePainterPanel(dynamicBook), "Texture");
    dynamicBook->AddPage(BuildPlantPainterPanel(dynamicBook), "Plant");
    paneSizer->Add(dynamicBook, 1, wxEXPAND | wxALL, 5);

    // === Obsługa zmiany typu painter ===
    painterTypeCombo->Bind(wxEVT_COMBOBOX,
                           [dynamicBook](wxCommandEvent& evt)
                           {
                               wxString sel = evt.GetString();
                               if (sel == "Height")
                                   dynamicBook->SetSelection(0);
                               else if (sel == "Texture")
                                   dynamicBook->SetSelection(1);
                               else if (sel == "Plant")
                                   dynamicBook->SetSelection(2);
                           });

    // Ustawiamy i integrujemy sizer
    pane->SetSizer(paneSizer);
    parentSizer->Add(collapsible, 0, wxEXPAND | wxALL, 5);
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

wxPanel* TerrainToolPanel::BuildHeightPainterPanel(wxWindow* parent)
{
    auto* panel = new wxPanel(parent);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // === Interpolation Method ===
    {
        auto* box   = new wxStaticBoxSizer(wxVERTICAL, panel, "Interpolation Method");
        auto* combo = new wxComboBox(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        // Placeholder: w przyszłości uzupełnisz metodami np. "Linear", "Smoothstep", "Cubic"...
        box->Add(combo, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);
    }

    // === Brush Type ===
    {
        auto* box   = new wxStaticBoxSizer(wxVERTICAL, panel, "Brush Type");
        auto* combo = new wxComboBox(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        // Placeholder: np. "Circle", "Square", "Noise", ...
        box->Add(combo, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);
    }

    // === Brush Size (float slider + text field) ===
    {
        auto* box = new wxStaticBoxSizer(wxVERTICAL, panel, "Brush Size");

        auto* slider = new wxSlider(panel, wxID_ANY, 50, 1, 200, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
        auto* text   = new wxTextCtrl(panel, wxID_ANY, "50.0");

        auto* hsizer = new wxBoxSizer(wxHORIZONTAL);
        hsizer->Add(slider, 1, wxEXPAND | wxRIGHT, 5);
        hsizer->Add(text, 0, wxALIGN_CENTER_VERTICAL);

        box->Add(hsizer, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);

        // --- Synchronizacja slider <-> text ---
        slider->Bind(wxEVT_SLIDER,
                     [slider, text](wxCommandEvent&)
                     {
                         float val = static_cast<float>(slider->GetValue());
                         text->ChangeValue(wxString::Format("%.2f", val));
                     });

        text->Bind(wxEVT_TEXT,
                   [slider](wxCommandEvent& evt)
                   {
                       double val;
                       if (evt.GetString().ToDouble(&val) && val >= 0.0)
                           slider->SetValue(wxRound(val));
                   });
    }

    // === Strength (float slider + text field, może być ujemne) ===
    {
        auto* box = new wxStaticBoxSizer(wxVERTICAL, panel, "Strength");

        auto* slider = new wxSlider(panel, wxID_ANY, 0, -100, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
        auto* text   = new wxTextCtrl(panel, wxID_ANY, "0.0");

        auto* hsizer = new wxBoxSizer(wxHORIZONTAL);
        hsizer->Add(slider, 1, wxEXPAND | wxRIGHT, 5);
        hsizer->Add(text, 0, wxALIGN_CENTER_VERTICAL);

        box->Add(hsizer, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);

        // --- Synchronizacja slider <-> text ---
        slider->Bind(wxEVT_SLIDER,
                     [slider, text](wxCommandEvent&)
                     {
                         float val = static_cast<float>(slider->GetValue()) / 10.0f;  // skala +/-10.0
                         text->ChangeValue(wxString::Format("%.2f", val));
                     });

        text->Bind(wxEVT_TEXT,
                   [slider](wxCommandEvent& evt)
                   {
                       double val;
                       if (evt.GetString().ToDouble(&val))
                       {
                           val = std::max(-10.0, std::min(10.0, val));
                           slider->SetValue(wxRound(val * 10.0));
                       }
                   });
    }

    // === Recalculate Normals ===
    {
        auto* btn = new wxButton(panel, wxID_ANY, "Recalculate Normals");
        sizer->Add(btn, 0, wxEXPAND | wxALL, 5);

        btn->Bind(wxEVT_BUTTON,
                  [this](wxCommandEvent&)
                  {
                      // TODO: tutaj podłącz logikę przeliczenia normalnych w terenie
                      wxLogMessage("Recalculate normals triggered");
                  });
    }

    panel->SetSizer(sizer);
    return panel;
}

wxPanel* TerrainToolPanel::BuildTexturePainterPanel(wxWindow* parent)
{
    auto* panel = new wxPanel(parent);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // === Interpolation Method ===
    {
        auto* box   = new wxStaticBoxSizer(wxVERTICAL, panel, "Interpolation Method");
        auto* combo = new wxComboBox(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        // TODO: uzupełnij listę metod w runtime
        box->Add(combo, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);
    }

    // === Brush Type ===
    {
        auto* box   = new wxStaticBoxSizer(wxVERTICAL, panel, "Brush Type");
        auto* combo = new wxComboBox(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        box->Add(combo, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);
    }

    // === Brush Size ===
    {
        auto* box    = new wxStaticBoxSizer(wxVERTICAL, panel, "Brush Size");
        auto* slider = new wxSlider(panel, wxID_ANY, 50, 1, 200);
        auto* text   = new wxTextCtrl(panel, wxID_ANY, "50.0");

        auto* hsizer = new wxBoxSizer(wxHORIZONTAL);
        hsizer->Add(slider, 1, wxEXPAND | wxRIGHT, 5);
        hsizer->Add(text, 0, wxALIGN_CENTER_VERTICAL);

        box->Add(hsizer, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);

        // Synchronizacja slider <-> text
        slider->Bind(wxEVT_SLIDER,
                     [slider, text](wxCommandEvent&)
                     {
                         float val = static_cast<float>(slider->GetValue());
                         text->ChangeValue(wxString::Format("%.2f", val));
                     });
        text->Bind(wxEVT_TEXT,
                   [slider](wxCommandEvent& evt)
                   {
                       double val;
                       if (evt.GetString().ToDouble(&val) && val >= 0.0)
                           slider->SetValue(wxRound(val));
                   });
    }

    // === Blend Strength ===
    {
        auto* box    = new wxStaticBoxSizer(wxVERTICAL, panel, "Blend Strength");
        auto* slider = new wxSlider(panel, wxID_ANY, 50, 0, 100);  // reprezentuje 0.0–1.0
        auto* text   = new wxTextCtrl(panel, wxID_ANY, "0.50");

        auto* hsizer = new wxBoxSizer(wxHORIZONTAL);
        hsizer->Add(slider, 1, wxEXPAND | wxRIGHT, 5);
        hsizer->Add(text, 0, wxALIGN_CENTER_VERTICAL);

        box->Add(hsizer, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);

        // Synchronizacja slider <-> text
        slider->Bind(wxEVT_SLIDER,
                     [slider, text](wxCommandEvent&)
                     {
                         float val = static_cast<float>(slider->GetValue()) / 100.0f;
                         text->ChangeValue(wxString::Format("%.2f", val));
                     });
        text->Bind(wxEVT_TEXT,
                   [slider](wxCommandEvent& evt)
                   {
                       double val;
                       if (evt.GetString().ToDouble(&val))
                       {
                           val = std::max(0.0, std::min(1.0, val));
                           slider->SetValue(wxRound(val * 100.0));
                       }
                   });
    }

    // === Clear Blend Map ===
    {
        auto* btn = new wxButton(panel, wxID_ANY, "Clear Blend Map");
        sizer->Add(btn, 0, wxEXPAND | wxALL, 5);
        btn->Bind(wxEVT_BUTTON,
                  [this](wxCommandEvent&)
                  {
                      wxLogMessage("Clear Blend Map clicked");
                      // TODO: implement actual clearing logic
                  });
    }

    // === Texture selection buttons ===
    {
        auto* box       = new wxStaticBoxSizer(wxVERTICAL, panel, "Texture Layers");
        auto* texPanel  = new wxPanel(panel);
        auto* wrapSizer = new wxWrapSizer(wxHORIZONTAL, wxWRAPSIZER_DEFAULT_FLAGS);
        texPanel->SetSizer(wrapSizer);

        const int texSize              = 64;
        std::vector<wxString> texNames = {"Grass", "Rock", "Sand", "Dirt", "Snow"};

        for (int i = 0; i < (int)texNames.size(); ++i)
        {
            wxBitmap bmp(texSize, texSize);
            wxMemoryDC dc(bmp);
            dc.SetBrush(*wxLIGHT_GREY_BRUSH);
            dc.Clear();
            dc.DrawText(texNames[i].Left(1), texSize / 2 - 5, texSize / 2 - 8);
            dc.SelectObject(wxNullBitmap);

            auto* texBtn = new TextureButton(texPanel, bmp, texNames[i], i);
            wrapSizer->Add(texBtn, 0, wxALL, 4);
        }

        texPanel->Layout();
        box->Add(texPanel, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);
    }

    panel->SetSizer(sizer);
    return panel;
}

wxPanel* TerrainToolPanel::BuildPlantPainterPanel(wxWindow* parent)
{
    auto* panel = new wxPanel(parent);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // === Brush Type ===
    {
        auto* box   = new wxStaticBoxSizer(wxVERTICAL, panel, "Brush Type");
        auto* combo = new wxComboBox(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        // TODO: w runtime uzupełnij listę dostępnych typów pędzla
        box->Add(combo, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);
    }

    // === Plant Texture ===
    {
        auto* box = new wxStaticBoxSizer(wxVERTICAL, panel, "Plant Texture");

        const int texSize = 64;
        wxBitmap bmp(texSize, texSize);  // placeholder
        wxMemoryDC dc(bmp);
        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
        dc.Clear();
        dc.DrawText("P", texSize / 2 - 5, texSize / 2 - 8);
        dc.SelectObject(wxNullBitmap);

        auto* texBtn = new TextureButton(panel, bmp, "PlantTexture", 0);

        texBtn->Bind(wxEVT_LEFT_DOWN,
                     [texBtn](wxMouseEvent&)
                     {
                         // Otwórz dialog pliku
                         wxFileDialog openFile(texBtn, "Select Plant Texture", "", "",
                                               "Images (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                         if (openFile.ShowModal() == wxID_OK)
                         {
                             wxString path = openFile.GetPath();
                             wxBitmap newBmp(path, wxBITMAP_TYPE_ANY);
                             if (newBmp.IsOk())
                             {
                                 texBtn->SetBitmap(newBmp);
                                 texBtn->SetToolTip(path);
                                 wxLogMessage("Selected plant texture: %s", path);
                             }
                         }
                     });

        box->Add(texBtn, 0, wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);
    }

    // === Density ===
    {
        auto* box    = new wxStaticBoxSizer(wxVERTICAL, panel, "Density");
        auto* slider = new wxSlider(panel, wxID_ANY, 50, 1, 100);
        auto* text   = new wxTextCtrl(panel, wxID_ANY, "50");

        auto* hsizer = new wxBoxSizer(wxHORIZONTAL);
        hsizer->Add(slider, 1, wxEXPAND | wxRIGHT, 5);
        hsizer->Add(text, 0, wxALIGN_CENTER_VERTICAL);

        box->Add(hsizer, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);

        slider->Bind(wxEVT_SLIDER,
                     [slider, text](wxCommandEvent&) { text->ChangeValue(wxString::Format("%d", slider->GetValue())); });
        text->Bind(wxEVT_TEXT,
                   [slider](wxCommandEvent& evt)
                   {
                       long val;
                       if (evt.GetString().ToLong(&val))
                           slider->SetValue(std::max(1L, std::min(100L, val)));
                   });
    }

    // === Randomness ===
    {
        auto* box    = new wxStaticBoxSizer(wxVERTICAL, panel, "Randomness (%)");
        auto* slider = new wxSlider(panel, wxID_ANY, 50, 0, 100);
        auto* text   = new wxTextCtrl(panel, wxID_ANY, "50");

        auto* hsizer = new wxBoxSizer(wxHORIZONTAL);
        hsizer->Add(slider, 1, wxEXPAND | wxRIGHT, 5);
        hsizer->Add(text, 0, wxALIGN_CENTER_VERTICAL);

        box->Add(hsizer, 0, wxEXPAND | wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);

        slider->Bind(wxEVT_SLIDER,
                     [slider, text](wxCommandEvent&) { text->ChangeValue(wxString::Format("%d", slider->GetValue())); });
        text->Bind(wxEVT_TEXT,
                   [slider](wxCommandEvent& evt)
                   {
                       long val;
                       if (evt.GetString().ToLong(&val))
                           slider->SetValue(std::max(0L, std::min(100L, val)));
                   });
    }

    panel->SetSizer(sizer);
    return panel;
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

void TerrainToolPanel::RefillTerrainObjectsCtrl()
{
    generatorFields.gameObjectIdCtrl->Clear();
    generatorFields.gameObjectIdCtrl->Append(
        "Create new terrain object", new TerrainObjectClientData(TerrainObjectClientData::Method::createNewTerrainObject));
    generatorFields.gameObjectIdCtrl->Append("Detect all terrain objects",
                                             new TerrainObjectClientData(TerrainObjectClientData::Method::getAllTerrains));
    DetectedTerrainGameObjectsAndAddToChoice();
    generatorFields.gameObjectIdCtrl->SetSelection(0);
}

void TerrainToolPanel::DetectedTerrainGameObjectsAndAddToChoice()
{
    auto terrains = scene.getComponentController().GetAllComponentsOfType<GameEngine::Components::TerrainRendererComponent>();
    for (const auto& terrain : terrains)
    {
        const auto& go = terrain->getParentGameObject();
        generatorFields.gameObjectIdCtrl->Append(go.GetName() + "(Id: " + std::to_string(go.GetId()) + ")",
                                                 new TerrainObjectClientData(go.GetId()));
    }
}

void TerrainToolPanel::GenerateTerrain(bool updateNoiseSeed)
{
    auto selectedGameObjectOptions = generatorFields.gameObjectIdCtrl->GetSelection();

    if (selectedGameObjectOptions != wxNOT_FOUND)
    {
        auto* data =
            dynamic_cast<TerrainObjectClientData*>(generatorFields.gameObjectIdCtrl->GetClientObject(selectedGameObjectOptions));
        if (data)
        {
            std::visit(visitor{[&](TerrainObjectClientData::Method method)
                               {
                                   switch (method)
                                   {
                                       case TerrainObjectClientData::Method::createNewTerrainObject:
                                           CreateAndGenerateTerrain(updateNoiseSeed);
                                           break;
                                       case TerrainObjectClientData::Method::getAllTerrains:
                                           GenerateForAllTerrains(updateNoiseSeed);
                                           break;
                                   }
                               },
                               [&](IdType gameObjectId) { GenerateTerrainForExistObject(updateNoiseSeed, gameObjectId); }},
                       data->GetValue());
        }
    }
}

void TerrainToolPanel::GenerateForAllTerrains(bool updateNoiseSeed)
{
    auto terrains = scene.getComponentController().GetAllComponentsOfType<GameEngine::Components::TerrainRendererComponent>();
    if (terrains.empty())
    {
        wxMessageBox("No single terrain object found in scene", "Error", wxICON_WARNING | wxOK);
        return;
    }

    GenerateTerrain(updateNoiseSeed, std::nullopt);
}

void TerrainToolPanel::CreateAndGenerateTerrain(bool updateNoiseSeed)
{
    if (auto gameObject = CreateTerrainObject())
        GenerateTerrain(updateNoiseSeed, gameObject->GetId());
}

GameEngine::GameObject* TerrainToolPanel::CreateTerrainObject()
{
    auto dlg = createEntryDialogWithSelectedText(this, "Enter terrain name:", "Crete new terrain object", "MyTerrain",
                                                 wxOK | wxCANCEL | wxCENTRE);

    std::string name{"MyTerrain"};

    while (true)
    {
        if (dlg->ShowModal() == wxID_CANCEL)
            return nullptr;

        auto value = dlg->GetValue().Trim(true).Trim(false);
        if (!value.IsEmpty())
        {
            name = value.ToStdString();
            break;
        }

        wxMessageBox("Value cannot be empty!", "Error", wxICON_WARNING | wxOK, dlg.get());
    }

    auto newTerrainGo = scene.CreateGameObject(name);
    newTerrainGo->AddComponent<GameEngine::Components::TerrainRendererComponent>();
    auto ptr = newTerrainGo.get();
    scene.AddGameObject(std::move(newTerrainGo));

    return ptr;
}
void TerrainToolPanel::GenerateTerrainForExistObject(bool updateNoiseSeed, IdType gameObjectId)
{
    GenerateTerrain(updateNoiseSeed, gameObjectId);
}

void TerrainToolPanel::GenerateTerrain(bool updateNoiseSeed, const std::optional<IdType>& gameObjectId)
{
    auto dlg = std::make_shared<LoadingDialog>(this, "Terrain generator", "Generating terrain...");

    std::thread(
        [=]()
        {
            try
            {
                GameEngine::TerrainHeightGenerator::EntryParamters entryParamters{
                    .bias = generatorFields.biasCtrl ? std::stof(generatorFields.biasCtrl->GetValue().ToStdString()) : 2.f,
                    .octaves =
                        generatorFields.octavesCtrl ? std::stoi(generatorFields.octavesCtrl->GetValue().ToStdString()) : 9u,
                    .perTerrainHeightMapsize = generatorFields.widthCtrl
                                                   ? vec2ui(std::stoi(generatorFields.widthCtrl->GetValue().ToStdString()))
                                                   : vec2ui{512, 512},
                    .gameObjectId            = gameObjectId};

                LOG_DEBUG << "Generate terrain " << entryParamters;

                GameEngine::TerrainHeightGenerator generator(scene.getComponentController(), entryParamters);

                if (updateNoiseSeed)
                {
                    generator.generateNoiseSeed();
                }

                generator.generateHeightMapsImage();
            }
            catch (...)
            {
                LOG_WARN << "Run TerrainHeightGenerator error.";
            }

            this->CallAfter([dlg]() { dlg->EndModal(wxID_OK); });
        })
        .detach();

    dlg->ShowModal();
}

void TerrainToolPanel::ImportFromMesh()
{
    wxFileDialog openFileDialog(this, "Open mesh to terrain import", ProjectManager::GetInstance().GetDataDir(), "",
                                "Mesh file (*.obj)|*.obj|All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    if (auto gameObject = CreateTerrainObject())
    {
        if (auto tc = gameObject->GetComponent<GameEngine::Components::TerrainRendererComponent>())
        {
            const auto& file = openFileDialog.GetPath().ToStdString();

            wxFileDialog saveFileDialog(this, "Save new height map as", ProjectManager::GetInstance().GetDataDir(), "",
                                        "Terrain files (*.terrain)|*.terrain|All files (*.*)|*.*", wxFD_SAVE);

            if (saveFileDialog.ShowModal() == wxID_CANCEL)
                return;

            uint32 heightmapResultuion{2048};
            try
            {
                {
                    if (generatorFields.widthCtrl)
                    {
                        heightmapResultuion = std::stoi(generatorFields.widthCtrl->GetValue().ToStdString());
                    }
                }
            }
            catch (...)
            {
                LOG_WARN << "heightmapResultuion parse error.";
            }

            auto dlg = std::make_shared<LoadingDialog>(this, "Mesh converter", "Convert mesh to terrain...");

            std::thread(
                [=, outputHeightMapPath = saveFileDialog.GetPath()]()
                {
                    if (auto heightmap =
                            tc->ConvertObjectToHeightMap(file, heightmapResultuion, outputHeightMapPath.ToStdString()))
                    {
                        tc->LoadTextures({GameEngine::Components::TerrainTexture{
                            .file = *heightmap, .tiledScale = 1.f, .type = GameEngine::TerrainTextureType::heightmap}});
                    }
                    this->CallAfter([dlg]() { dlg->EndModal(wxID_OK); });
                })
                .detach();

            dlg->ShowModal();
        }
    }
}