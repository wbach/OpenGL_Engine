#include "TerrainToolPanel.h"

#include <GameEngine/Components/ComponentController.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h>
#include <GameEngine/DebugTools/Painter/TerrainHeightGenerator.h>
#include <wx/collpane.h>
#include <wx/combobox.h>

#include <GameEngine/Scene/Scene.hpp>
#include <optional>

#include "EditorUitls.h"
#include "Logger/Log.h"
#include "Types.h"

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
    generatorFields.gameObjectIdCtrl = new wxTextCtrl(pane, wxID_ANY, "---");
    auto* goSizer                    = new wxStaticBoxSizer(wxVERTICAL, pane, "GameObjectId");
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

    generatorFields.widthCtrl = new wxComboBox(pane, wxID_ANY, "512", wxDefaultPosition, wxDefaultSize, widthChoices, wxCB_READONLY);
    widthSizer->Add(generatorFields.widthCtrl, 1, wxEXPAND | wxRIGHT, 5);

    paneSizer->Add(widthSizer, 0, wxEXPAND | wxALL, 5);

    // Buttons
    wxButton* btnSameSeed = new wxButton(pane, wxID_ANY, "Generate with the same seed");
    btnSameSeed->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { GenerateTerrain(false); });
    paneSizer->Add(btnSameSeed, 0, wxEXPAND | wxALL, 5);

    wxButton* btnNewSeed = new wxButton(pane, wxID_ANY, "Generate terrain with new seed");
    btnNewSeed->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { GenerateTerrain(true); });
    paneSizer->Add(btnNewSeed, 0, wxEXPAND | wxALL, 5);

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

void TerrainToolPanel::GenerateTerrain(bool updateNoiseSeed)
{
    std::optional<IdType> gameObjectId;

    try
    {
        gameObjectId = generatorFields.gameObjectIdCtrl
                           ? std::make_optional(std::stoi(generatorFields.gameObjectIdCtrl->GetValue().ToStdString()))
                           : std::nullopt;
    }
    catch (...)
    {
        LOG_DEBUG << "gameObjectIdCtrl invalid parse.";
    }

    try
    {
        if (not gameObjectId)
        {
            auto terrains =
                scene.getComponentController().GetAllComponentsOfType<GameEngine::Components::TerrainRendererComponent>();
            if (terrains.empty())
            {
                auto dlg =
                    createEntryDialogWithSelectedText(this, "Enter terrain name:", "Any terrain found, create new gameObject",
                                                      "MyTerrain", wxOK | wxCANCEL | wxCENTRE);

                int answer = wxMessageBox("Any terrain found, create new gameObject  with terrain component?", "Confirmation",
                                          wxYES_NO | wxICON_QUESTION);

                if (answer == wxNO)
                    return;

                std::string name{"MyTerrain"};

                while (true)
                {
                    if (dlg->ShowModal() == wxID_CANCEL)
                        return;

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
                gameObjectId = newTerrainGo->GetId();
                scene.AddGameObject(std::move(newTerrainGo));
            }
        }

        GameEngine::TerrainHeightGenerator::EntryParamters entryParamters{
            .bias    = generatorFields.biasCtrl ? std::stof(generatorFields.biasCtrl->GetValue().ToStdString()) : 2.f,
            .octaves = generatorFields.octavesCtrl ? std::stoi(generatorFields.octavesCtrl->GetValue().ToStdString()) : 9u,
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
}