#include "TerrainToolPanel.h"

#include <GameEngine/Components/ComponentController.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h>
#include <GameEngine/DebugTools/Painter/CircleBrush.h>
#include <GameEngine/DebugTools/Painter/HeightPainter.h>
#include <GameEngine/DebugTools/Painter/Interpolation.h>
#include <GameEngine/DebugTools/Painter/Painter.h>
#include <GameEngine/DebugTools/Painter/TerrainHeightGenerator.h>
#include <GameEngine/DebugTools/Painter/TexturePainter.h>
#include <GameEngine/Engine/EngineContext.h>
#include <GameEngine/Resources/File.h>
#include <GameEngine/Resources/Models/Material.h>
#include <GameEngine/Resources/Models/Primitive.h>
#include <GameEngine/Resources/Textures/GeneralTexture.h>
#include <Logger/Log.h>
#include <Types.h>
#include <Utils/Variant.h>
#include <wx/busyinfo.h>
#include <wx/collpane.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/popupwin.h>
#include <wx/simplebook.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/wrapsizer.h>

#include <GameEngine/Components/Renderer/Entity/RendererComponent.hpp>
#include <GameEngine/Scene/Scene.hpp>
#include <algorithm>
#include <limits>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "TerrainSelectionDialog.h"
#include "TextureButton.h"
#include "TexturePickerPopup.h"
#include "WxEditor/EditorUitls.h"
#include "WxEditor/LoadingDialog.h"
#include "WxEditor/ProjectManager.h"
#include "WxEditor/WxChoiceClientData/WxClientData.h"

namespace
{
enum class BrushTypes
{
    Circle
};

enum class GenerateTerrainTarget
{
    newTerrain,
    forAllTerrains
};

using TerrainObjectClientData = WxClientData<std::variant<GenerateTerrainTarget, IdType>>;

GameEngine::TerrainPainter::Dependencies GetPainterDependencies(GameEngine::Scene& scene)
{
    auto engineContext = scene.getEngineContext();
    if (not engineContext)
        throw std::runtime_error("Scene not init. engineContext is null");

    return GameEngine::TerrainPainter::Dependencies{engineContext->GetInputManager(), engineContext->GetThreadSync(),
                                                    scene.GetCamera(), engineContext->GetRenderersManager().GetProjection(),
                                                    scene.getComponentController()};
}

template <typename EnumType>
wxComboBox* CreateEnumComboBox(wxWindow* parent, wxSizer* sizer, const std::string& label, EnumType defaultValue,
                               std::function<void()> onChangeCallback)
{
    // 1 Zbierz nazwy enumow do wxArrayString
    wxArrayString items;
    for (const auto& name : magic_enum::enum_names<EnumType>())
    {
        items.Add(std::string(name));
    }

    // 2 Domyslna wartosc
    std::string defaultName = std::string(magic_enum::enum_name(defaultValue));

    // 3 Stworz statyczny box i combo box
    auto* box   = new wxStaticBoxSizer(wxVERTICAL, parent, label);
    auto* combo = new wxComboBox(parent, wxID_ANY, defaultName, wxDefaultPosition, wxDefaultSize, items, wxCB_READONLY);

    box->Add(combo, 0, wxEXPAND | wxALL, 5);
    sizer->Add(box, 0, wxEXPAND | wxALL, 5);

    // 4 Podlacz callback
    combo->Bind(wxEVT_COMBOBOX, [onChangeCallback](const auto&) { onChangeCallback(); });

    return combo;
}

template <typename T>
wxTextCtrl* CreateSlider(wxWindow* parent, wxSizer* sizer, const std::string& label, T minValue, T maxValue, T defaultValue,
                         std::function<void(T)> onValueChanged = nullptr, int sliderResolution = 100)
{
    static_assert(std::is_arithmetic_v<T>, "CreateSlider requires arithmetic type");

    // 1 Statyczny box
    auto* box = new wxStaticBoxSizer(wxVERTICAL, parent, label);

    // 2 Slider
    int sliderValue;
    if constexpr (std::is_floating_point_v<T>)
    {
        sliderValue = static_cast<int>((defaultValue - minValue) / (maxValue - minValue) * sliderResolution);
    }
    else
    {
        sliderValue      = static_cast<int>(defaultValue - minValue);
        sliderResolution = static_cast<int>(maxValue - minValue);
    }

    auto* slider = new wxSlider(parent, wxID_ANY, sliderValue, 0, sliderResolution);

    // 3 TextCtrl z formatowaniem
    std::string textValue;
    if constexpr (std::is_integral_v<T>)
    {
        textValue = std::to_string(defaultValue);
    }
    else
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", static_cast<double>(defaultValue));
        textValue = buf;
    }

    auto* text = new wxTextCtrl(parent, wxID_ANY, textValue, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    // 4 Poziomy sizer
    auto* hsizer = new wxBoxSizer(wxHORIZONTAL);
    hsizer->Add(slider, 1, wxEXPAND | wxRIGHT, 5);
    hsizer->Add(text, 0, wxALIGN_CENTER_VERTICAL);

    box->Add(hsizer, 0, wxEXPAND | wxALL, 5);
    sizer->Add(box, 0, wxEXPAND | wxALL, 5);

    // 5 Synchronizacja slider -> text
    slider->Bind(wxEVT_SLIDER,
                 [slider, text, minValue, maxValue, sliderResolution, onValueChanged](const auto&)
                 {
                     T val;
                     if constexpr (std::is_floating_point_v<T>)
                     {
                         val = minValue + static_cast<T>(slider->GetValue()) / sliderResolution * (maxValue - minValue);
                         text->ChangeValue(wxString::Format("%.2f", static_cast<double>(val)));
                     }
                     else
                     {
                         val = static_cast<T>(slider->GetValue()) + minValue;
                         text->ChangeValue(wxString::Format("%d", static_cast<int>(val)));
                     }
                     if (onValueChanged)
                         onValueChanged(val);
                 });

    // 6 Synchronizacja text -> slider (Enter)
    text->Bind(wxEVT_TEXT_ENTER,
               [slider, minValue, maxValue, sliderResolution, onValueChanged](wxCommandEvent& evt)
               {
                   T val{};
                   bool valid = false;

                   if constexpr (std::is_integral_v<T>)
                   {
                       long tmp;
                       valid = evt.GetString().ToLong(&tmp);
                       if (valid)
                           val = static_cast<T>(std::clamp(tmp, static_cast<long>(minValue), static_cast<long>(maxValue)));
                   }
                   else
                   {
                       double tmp;
                       valid = evt.GetString().ToDouble(&tmp);
                       if (valid)
                           val = static_cast<T>(std::clamp(static_cast<T>(tmp), minValue, maxValue));
                   }

                   if (valid)
                   {
                       int sliderPos;
                       if constexpr (std::is_floating_point_v<T>)
                       {
                           sliderPos = static_cast<int>((val - minValue) / (maxValue - minValue) * sliderResolution);
                       }
                       else
                       {
                           sliderPos = static_cast<int>(val - minValue);
                       }

                       if (slider->GetValue() != sliderPos)
                       {
                           slider->SetValue(sliderPos);
                           if (onValueChanged)
                               onValueChanged(val);
                       }
                   }
               });

    return text;
}

}  // namespace

// clang-format off
wxBEGIN_EVENT_TABLE(TerrainToolPanel, wxPanel)
    EVT_BUTTON(wxID_CLOSE, TerrainToolPanel::OnClose)
wxEND_EVENT_TABLE()

TerrainToolPanel::TerrainToolPanel(wxWindow* parent, GameEngine::Scene& scene, int width)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(width, -1), wxBORDER_NONE | wxTAB_TRAVERSAL | wxSTAY_ON_TOP )
    , scene{scene}
// clang-format on
{
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

void TerrainToolPanel::Cleanup()
{
    if (sceneEventSubId)
    {
        scene.UnSubscribeForSceneEvent(*sceneEventSubId);
        sceneEventSubId.reset();
    }
}

void TerrainToolPanel::SelectedPainterTexture(wxMouseEvent& event)
{
    auto getTerrainComponent = [this](int id) -> GameEngine::Components::TerrainRendererComponent*
    {
        auto terrains = scene.getComponentController().GetAllComponentsOfType<GameEngine::Components::TerrainRendererComponent>();
        auto goId     = static_cast<IdType>(id);

        auto iter = std::find_if(terrains.begin(), terrains.end(),
                                 [goId](const auto& tc) { return tc->getParentGameObject().GetId() == goId; });

        if (iter != terrains.end())
        {
            return (*iter);
        }

        return nullptr;
    };
    auto onSelect = [this](const GameEngine::File& file)
    {
        painterFields.texturePainterFields.selectedTextureButton->SetBitmap(file);
        painterFields.texturePainterFields.selectedTextureFile = file;
        OnUpdatePainterParam();
    };

    auto onAdd = [this]()
    {
        if (auto maybeFileSelected = painterFields.texturePainterFields.selectedTextureButton->SelectFileDialog())
        {
            SetToolTip(maybeFileSelected->GetBaseName());
            auto& selectedFile = painterFields.texturePainterFields.selectedTextureFile;
            selectedFile       = maybeFileSelected;

            painterFields.texturePainterFields.selectedTextureButton->SetBitmap(*selectedFile);

            auto& textures = painterFields.texturePainterFields.textures;
            if (std::find(textures.begin(), textures.end(), selectedFile) == textures.end())
                textures.push_back(*selectedFile);
            OnUpdatePainterParam();
        }
    };

    auto onChange =
        [this, getTerrainComponent, onSelect](const std::optional<GameEngine::File>& oldFile, const GameEngine::File& newFile)
    {
        auto isAnyTerrainUseTexture = [this](const GameEngine::File& file)
        {
            auto terrains =
                scene.getComponentController().GetAllComponentsOfType<GameEngine::Components::TerrainRendererComponent>();

            auto iter = std::find_if(terrains.begin(), terrains.end(),
                                     [&](auto componentPtr)
                                     {
                                         const auto& textures = componentPtr->GetTextures();

                                         for (const auto& [type, texture] : textures)
                                         {
                                             if (GameEngine::isPaintAbleTexture(type))
                                             {
                                                 if (auto gt = dynamic_cast<GameEngine::GeneralTexture*>(texture))
                                                 {
                                                     if (gt->GetFile() == file)
                                                         return true;
                                                 }
                                             }
                                         }

                                         return false;
                                     });

            return (iter != terrains.end());
        };

        if (not oldFile or not isAnyTerrainUseTexture(*oldFile))
        {
            onSelect(newFile);
            return;
        }

        TerrainSelectionDialog dlg(this, scene.getComponentController(),
                                   "Texture is used in some terrains. What change do you want to do?");
        if (dlg.ShowModal() == wxID_OK)
        {
            auto selection = dlg.GetChoice()->GetSelection();
            auto* data     = dynamic_cast<WxClientData<int>*>(dlg.GetChoice()->GetClientObject(selection));
            if (data)
            {
                auto terrains =
                    scene.getComponentController().GetAllComponentsOfType<GameEngine::Components::TerrainRendererComponent>();
                int value = data->GetValue();

                if (value == -1)
                {
                    for (auto& terrain : terrains)
                    {
                        terrain->ChangeTexture(*oldFile, newFile);
                    }
                }
                else
                {
                    if (auto component = getTerrainComponent(value))
                    {
                        component->ChangeTexture(*oldFile, newFile);
                    }
                }
            }
            onSelect(newFile);
        }
    };

    auto onRemove = [this](const GameEngine::File& file)
    {
        auto& textures = painterFields.texturePainterFields.textures;
        auto iter      = std::find(textures.begin(), textures.end(), file);
        if (iter != textures.end())
        {
            textures.erase(iter);
        }

        painterFields.texturePainterFields.selectedTextureButton->Reset();
        painterFields.texturePainterFields.selectedTextureFile.reset();
        DisablePainter();

        TerrainSelectionDialog dlg(this, scene.getComponentController(), "Do you wanna remove this texture from terrain?");
        if (dlg.ShowModal() == wxID_OK)
        {
            auto selection = dlg.GetChoice()->GetSelection();
            auto* data     = dynamic_cast<WxClientData<int>*>(dlg.GetChoice()->GetClientObject(selection));
            if (data)
            {
                auto terrains =
                    scene.getComponentController().GetAllComponentsOfType<GameEngine::Components::TerrainRendererComponent>();
                int value = data->GetValue();
                if (value == -1)
                {
                    for (auto terrain : terrains)
                    {
                        terrain->RemoveTexture(file);
                    }
                }
                else
                {
                    auto goId = static_cast<IdType>(value);

                    auto iter = std::find_if(terrains.begin(), terrains.end(),
                                             [goId](const auto& tc) { return tc->getParentGameObject().GetId() == goId; });

                    if (iter != terrains.end())
                    {
                        (**iter).RemoveTexture(file);
                    }
                    else
                    {
                        wxLogMessage(("Something goes wrong. go with not found! GoId : " + std::to_string(goId)).c_str());
                    }
                }
            }
        }
    };

    std::vector<TexturePickerPopup::TexureInfo> textures;
    for (const auto& texture : painterFields.texturePainterFields.textures)
    {
        textures.push_back(TexturePickerPopup::TexureInfo{.file = texture});
    }

    auto getObjectNameWithId = [](const GameEngine::GameObject& go)
    { return go.GetName() + " (Id: " + std::to_string(go.GetId()) + ")"; };

    auto terrains = scene.getComponentController().GetAllComponentsOfType<GameEngine::Components::TerrainRendererComponent>();
    for (const auto& terrain : terrains)
    {
        for (const auto& [type, texture] : terrain->GetTextures())
        {
            if (GameEngine::isPaintAbleTexture(type) and texture->GetFile())
            {
                auto iter = std::find_if(textures.begin(), textures.end(),
                                         [t = texture->GetFile()](const auto& info)
                                         { return info.file.GetAbsolutePath() == t->GetAbsolutePath(); });

                if (iter != textures.end())
                {
                    iter->usedIn.push_back(getObjectNameWithId(terrain->GetParentGameObject()));
                }
                else
                {
                    textures.push_back(TexturePickerPopup::TexureInfo{
                        .file = texture->GetFile().value(), .usedIn = {getObjectNameWithId(terrain->GetParentGameObject())}});
                }
            }
        }
    }

    auto popup = new TexturePickerPopup(this, textures, onSelect, onAdd, onChange, onRemove);

    popup->Position(wxGetMousePosition(), wxSize(0, 0));
    popup->Popup();
}

void TerrainToolPanel::BuildUI()
{
    auto* mainSizer   = new wxBoxSizer(wxVERTICAL);
    auto* headerSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* title = new wxStaticText(this, wxID_ANY, "Terrain Tools");
    headerSizer->Add(title, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);

    // Rozpycha przycisk w prawa strone
    headerSizer->AddStretchSpacer();

    wxButton* closeBtn = new wxButton(this, wxID_CLOSE, "X", wxDefaultPosition, wxSize(40, 20));
    headerSizer->Add(closeBtn, 0, wxALL, 2);

    mainSizer->Add(headerSizer, 0, wxEXPAND);

    // Ramka generatora
    BuildTerrainGeneratorUI(mainSizer);
    BuildTerrainPainterUI(mainSizer);
    // BuildPlantPainterUI

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

    painterFields.enableDisablePainterButton = new wxButton(pane, wxID_ANY, "Enable painter");
    paneSizer->Add(painterFields.enableDisablePainterButton, 0, wxEXPAND | wxALL, 5);
    painterFields.enableDisablePainterButton->Bind(wxEVT_BUTTON,
                                                   [this](wxCommandEvent& e)
                                                   {
                                                       if (painterFields.terrainPainter_)
                                                       {
                                                           DisablePainter();
                                                       }
                                                       else
                                                       {
                                                           EnablePainter();
                                                       }

                                                       e.Skip();
                                                   });

    // === Painter Type wybor ===
    auto* painterTypeBox = new wxStaticBoxSizer(wxVERTICAL, pane, "Painter Type");
    wxArrayString painterTypes;
    painterTypes.Add("Height");
    painterTypes.Add("Texture");
    painterTypes.Add("Plant");

    painterFields.painterTypeCtrl =
        new wxComboBox(pane, wxID_ANY, "Height", wxDefaultPosition, wxDefaultSize, painterTypes, wxCB_READONLY);
    painterTypeBox->Add(painterFields.painterTypeCtrl, 0, wxEXPAND | wxALL, 5);
    paneSizer->Add(painterTypeBox, 0, wxEXPAND | wxALL, 5);
    painterFields.enableDisablePainterButton->SetLabelText("Enable " + painterFields.painterTypeCtrl->GetValue() + " painter");

    // === Dynamiczna sekcja z painterami ===
    auto* dynamicBook = new wxSimplebook(pane, wxID_ANY);
    dynamicBook->AddPage(BuildHeightPainterPanel(dynamicBook), "Height");
    dynamicBook->AddPage(BuildTexturePainterPanel(dynamicBook), "Texture");
    dynamicBook->AddPage(BuildPlantPainterPanel(dynamicBook), "Plant");
    paneSizer->Add(dynamicBook, 1, wxEXPAND | wxALL, 5);

    // === Obsluga zmiany typu painter ===
    painterFields.painterTypeCtrl->Bind(wxEVT_COMBOBOX,
                                        [this, dynamicBook](wxCommandEvent& evt)
                                        {
                                            wxString sel = evt.GetString();
                                            if (sel == "Height")
                                                dynamicBook->SetSelection(0);
                                            else if (sel == "Texture")
                                                dynamicBook->SetSelection(1);
                                            else if (sel == "Plant")
                                                dynamicBook->SetSelection(2);
                                            painterFields.enableDisablePainterButton->SetLabelText(
                                                "Enable " + painterFields.painterTypeCtrl->GetValue() + " painter");
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
        painterFields.heightPainterFields.interpolation = CreateEnumComboBox<GameEngine::InterpolationType>(
            panel, sizer, "Interpolation Method", GameEngine::InterpolationType::Smooth, [this]() { OnUpdatePainterParam(); });
    }
    // === Brush Type ===
    painterFields.heightPainterFields.brushType =
        CreateEnumComboBox<BrushTypes>(panel, sizer, "Brush Type", BrushTypes::Circle, [this]() { OnUpdatePainterParam(); });

    // === Brush Size ===
    painterFields.heightPainterFields.brushSize =
        CreateSlider<float>(panel, sizer, "Brush Size", 1.0, 20.0, 1.0, [this](int val) { OnUpdatePainterParam(); });

    // === Blend Strength ===
    painterFields.heightPainterFields.strength =
        CreateSlider<float>(panel, sizer, "Brush Strength", -10.0f, 10.0f, 1.f, [this](int val) { OnUpdatePainterParam(); });

    // === Recalculate Normals ===
    {
        auto* btn = new wxButton(panel, wxID_ANY, "Recalculate Normals");
        sizer->Add(btn, 0, wxEXPAND | wxALL, 5);

        btn->Bind(wxEVT_BUTTON,
                  [this](wxCommandEvent&)
                  {
                      if (auto heightPainter = dynamic_cast<GameEngine::HeightPainter*>(painterFields.terrainPainter_.get()))
                      {
                          heightPainter->RecalculateTerrainNormals();
                      }
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
    painterFields.texturePainterFields.interpolation = CreateEnumComboBox<GameEngine::InterpolationType>(
        panel, sizer, "Interpolation Method", GameEngine::InterpolationType::Linear, [this]() { OnUpdatePainterParam(); });

    // === Brush Type ===
    painterFields.texturePainterFields.brushType =
        CreateEnumComboBox<BrushTypes>(panel, sizer, "Brush Type", BrushTypes::Circle, [this]() { OnUpdatePainterParam(); });

    // === Brush Size ===
    painterFields.texturePainterFields.brushSize =
        CreateSlider<float>(panel, sizer, "Brush Size", 1.0, 20.0, 1.0, [this](int val) { OnUpdatePainterParam(); });

    // === Blend Strength ===
    painterFields.texturePainterFields.strength =
        CreateSlider<float>(panel, sizer, "Brush Strength", 1.0, 20.0, 1.0, [this](int val) { OnUpdatePainterParam(); });

    // === Selected Texture ===
    {
        auto* box    = new wxStaticBoxSizer(wxVERTICAL, panel, "Selected Texture");
        auto* texBtn = new TextureButton(panel, std::nullopt, TextureButton::MenuOption::None,
                                         [this](const GameEngine::File& file)
                                         { painterFields.texturePainterFields.selectedTextureFile = file; });

        texBtn->Bind(wxEVT_LEFT_DOWN, &TerrainToolPanel::SelectedPainterTexture, this);
        texBtn->Bind(wxEVT_RIGHT_DOWN, &TerrainToolPanel::SelectedPainterTexture, this);

        box->Add(texBtn, 0, wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);
        painterFields.texturePainterFields.selectedTextureButton = texBtn;
    }

    panel->SetSizer(sizer);
    return panel;
}

wxPanel* TerrainToolPanel::BuildPlantPainterPanel(wxWindow* parent)
{
    auto* panel = new wxPanel(parent);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // === Brush Type ===
    painterFields.plantPainterFields.brushType =
        CreateEnumComboBox<BrushTypes>(panel, sizer, "Brush Type", BrushTypes::Circle, [this]() { OnUpdatePainterParam(); });

    // === Brush Size ===
    painterFields.plantPainterFields.brushSize =
        CreateSlider<float>(panel, sizer, "Brush Size", 1.0, 20.0, 1.0, [this](int val) { OnUpdatePainterParam(); });

    // === Density ===
    painterFields.plantPainterFields.density =
        CreateSlider<float>(panel, sizer, "Density", 1.0, 20.0, 1.0, [this](int val) { OnUpdatePainterParam(); });

    // === Randomness ===
    painterFields.plantPainterFields.randomness =
        CreateSlider<float>(panel, sizer, "Randomness", 1.0, 20.0, 1.0, [this](int val) { OnUpdatePainterParam(); });

    // === Plant Texture ===
    {
        auto* box    = new wxStaticBoxSizer(wxVERTICAL, panel, "Plant Texture");
        auto* texBtn = new TextureButton(
            panel, std::nullopt, TextureButton::MenuOption::Change,
            [this](const GameEngine::File& file) { painterFields.plantPainterFields.selectedTextureFile = file; }, nullptr);

        box->Add(texBtn, 0, wxALL, 5);
        sizer->Add(box, 0, wxEXPAND | wxALL, 5);
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
    event.Skip();
}

void TerrainToolPanel::RefillTerrainObjectsCtrl()
{
    generatorFields.gameObjectIdCtrl->Clear();
    generatorFields.gameObjectIdCtrl->Append("Create new terrain object",
                                             new TerrainObjectClientData(GenerateTerrainTarget::newTerrain));
    generatorFields.gameObjectIdCtrl->Append("Detect all terrain objects",
                                             new TerrainObjectClientData(GenerateTerrainTarget::forAllTerrains));
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
            std::visit(visitor{[&](GenerateTerrainTarget method)
                               {
                                   switch (method)
                                   {
                                       case GenerateTerrainTarget::newTerrain:
                                           CreateAndGenerateTerrain(updateNoiseSeed);
                                           break;
                                       case GenerateTerrainTarget::forAllTerrains:
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

GameEngine::GameObject* TerrainToolPanel::createPainterVisualizationObject()
{
    auto& resourceManager = scene.GetResourceManager();
    auto model            = resourceManager.GetPrimitives(GameEngine::PrimitiveType::Sphere);
    auto obj              = scene.CreateGameObject(std::string("TerrainHeightPainterVisualization"));
    obj->AddComponent<GameEngine::Components::RendererComponent>().AddModel(model).AddCustomMaterial(
        GameEngine::Material{.diffuse = Color(120, 205, 255)});
    auto result = obj.get();
    result->SetLocalPosition(vec3(std::numeric_limits<float>::max()));
    scene.AddGameObject(std::move(obj));
    return result;
}

void TerrainToolPanel::OnUpdatePainterParam()
{
    if (painterFields.terrainPainter_)
    {
        painterFields.terrainPainter_.reset();
        EnablePainter();
    }
}

void TerrainToolPanel::EnablePainter()
{
    if (painterFields.terrainPainter_)
    {
        LOG_WARN << "Painter already enabled.";
        return;
    }

    auto getInterpolation = [&](wxComboBox* interpolationBox)
    {
        if (auto value = magic_enum::enum_cast<GameEngine::InterpolationType>(interpolationBox->GetValue().ToStdString()))
        {
            return *value;
        }
        else
        {
            LOG_WARN << "StepInterpolation parse error";
        }

        return GameEngine::InterpolationType::Smooth;
    };

    auto getStrength = [&](wxTextCtrl* strengthTextCtrl)
    {
        try
        {
            return strengthTextCtrl ? std::stof(strengthTextCtrl->GetValue().ToStdString()) : 0.f;
        }
        catch (...)
        {
            LOG_WARN << "getStrength parse error";
        }
        return 0.f;
    };

    auto getRadius = [&](wxTextCtrl* brushSizeCtrl)
    {
        if (not brushSizeCtrl)
            return GameEngine::WorldSpaceBrushRadius{1.f};

        try
        {
            return GameEngine::WorldSpaceBrushRadius{std::stof(brushSizeCtrl->GetValue().ToStdString())};
        }
        catch (...)
        {
            LOG_WARN << "getStrength parse error";
        }
        return GameEngine::WorldSpaceBrushRadius{1.f};
    };

    int selection = painterFields.painterTypeCtrl->GetSelection();
    try
    {
        switch (selection)
        {
            case 0:
            {
                LOG_DEBUG << "EnablePainter";

                const auto& heightPainterFields = painterFields.heightPainterFields;

                auto interpolation = getInterpolation(heightPainterFields.interpolation);
                auto strength      = getStrength(heightPainterFields.strength);
                auto radius        = getRadius(heightPainterFields.brushSize);

                auto circleBrush = std::make_unique<GameEngine::CircleBrush>(GameEngine::makeInterpolation(interpolation), radius,
                                                                             strength / 1000.f);
                auto heightPainter =
                    std::make_unique<GameEngine::HeightPainter>(GetPainterDependencies(scene), std::move(circleBrush));

                if (not visualizationObject)
                    visualizationObject = createPainterVisualizationObject();

                visualizationObject->SetWorldScale(vec3(radius.value, 0.1f, radius.value));
                heightPainter->SetOnPointChange(
                    [this](const auto& terrainPoint)
                    {
                        if (terrainPoint)
                            visualizationObject->SetWorldPosition(terrainPoint->pointOnTerrain);
                        else
                            visualizationObject->SetWorldPosition(vec3(std::numeric_limits<float>::max()));
                    });
                painterFields.terrainPainter_ = std::move(heightPainter);
                painterFields.terrainPainter_->Start();
            }
            break;
            case 1:
            {
                const auto& texturePainterFields = painterFields.texturePainterFields;
                if (not texturePainterFields.selectedTextureFile)
                {
                    LOG_WARN << "No texture file selected.";
                    return;
                }
                auto interpolation = getInterpolation(texturePainterFields.interpolation);
                auto strength      = getStrength(texturePainterFields.strength);
                auto radius        = getRadius(texturePainterFields.brushSize);

                auto circleBrush =
                    std::make_unique<GameEngine::CircleBrush>(GameEngine::makeInterpolation(interpolation), radius, strength);

                painterFields.terrainPainter_ = std::make_unique<GameEngine::TexturePainter>(
                    GetPainterDependencies(scene), scene.GetResourceManager().GetTextureLoader(), std::move(circleBrush),
                    *texturePainterFields.selectedTextureFile);

                painterFields.terrainPainter_->Start();
            }
            break;
            case 2:
            {
                // GameEngine::Components::GrassRendererComponent* grassRendererComponent{nullptr};
                // if (grassRendererComponent)
                //     painterFields.terrainPainter_ =
                //         std::make_unique<GameEngine::PlantPainter>(GetPainterEntryParameters(scene), *grassRendererComponent);
            }
            break;
        }
    }
    catch (const std::runtime_error& error)
    {
        LOG_ERROR << error.what();
        wxMessageBox(error.what(), "Error", wxICON_WARNING | wxOK);
    }
    catch (...)
    {
        LOG_ERROR << "Enable painter error";
        wxMessageBox("Enable painter error", "Error", wxICON_WARNING | wxOK);
    }

    if (painterFields.terrainPainter_)
    {
        painterFields.terrainPainter_->SetNotifyMessageFunc([](const auto& message) { wxLogMessage(message.c_str()); });
    }

    painterFields.enableDisablePainterButton->SetLabelText("Disable " + painterFields.painterTypeCtrl->GetValue() + " painter");
}

void TerrainToolPanel::DisablePainter()
{
    painterFields.terrainPainter_.reset();
    painterFields.enableDisablePainterButton->SetLabelText("Enable " + painterFields.painterTypeCtrl->GetValue() + " painter");

    if (visualizationObject)
        visualizationObject->SetWorldPosition(vec3(std::numeric_limits<float>::max()));
}