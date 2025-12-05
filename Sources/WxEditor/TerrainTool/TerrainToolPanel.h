#pragma once

#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Resources/File.h>
#include <GameEngine/Scene/SceneEvents.h>
#include <Types.h>
#include <Utils/Image/Image.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/wx.h>

#include <optional>
#include <unordered_map>
#include <vector>

#include "Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "WxEditor/Commands/TextureCommandEntry.h"

namespace GameEngine
{
class Scene;
class Painter;
class HeightPainter;
class TexturePainter;
class PlantPainter;
}  // namespace GameEngine

struct ColorPickerResult
{
    wxPanel* colorPreview;
    std::array<wxTextCtrl*, 3> randomnessTexts;
    wxColour currentColor;
};

class TextureButton;

class TerrainToolPanel : public wxScrolledWindow
{
public:
    TerrainToolPanel(wxWindow* parent, GameEngine::Scene&, int width);
    ~TerrainToolPanel() override;

    void Cleanup();

    void ShowPanel(bool show);
    bool IsVisible() const
    {
        return isVisible;
    }

private:
    void OnClose(wxCommandEvent&);
    void BuildUI();
    void BuildTerrainGeneratorUI(wxSizer*);
    void BuildTerrainPainterUI(wxSizer*);
    wxPanel* BuildHeightPainterPanel(wxWindow*);
    wxPanel* BuildTexturePainterPanel(wxWindow*);
    wxPanel* BuildPlantPainterPanel(wxWindow*);
    void GenerateTerrain(bool);
    void GenerateTerrain(bool, const std::optional<IdType>&);
    GameEngine::GameObject* CreateTerrainObject();
    void CreateAndGenerateTerrain(bool);
    void GenerateTerrainForExistObject(bool, IdType);
    void GenerateForAllTerrains(bool);
    void RefillTerrainObjectsCtrl();
    void DetectedTerrainGameObjectsAndAddToChoice();
    void ImportFromMesh();
    void EnablePainter();
    void DisablePainter();
    std::unique_ptr<GameEngine::HeightPainter> CreateHeightPainter();
    std::unique_ptr<GameEngine::TexturePainter> CreateTexturePainter();
    std::unique_ptr<GameEngine::PlantPainter> CreatePlantPainter();
    void OnUpdatePainterParam();
    void SelectedPainterTexture(wxMouseEvent&);
    GameEngine::GameObject* createPainterVisualizationObject();
    void GeneratePlantsBasedOnTerrainTexture();
    void GeneratePlantsBasedOnTerrainSpecyfic();

    template <typename T>
    void ProcessEvent(const T&)
    {
        RefillTerrainObjectsCtrl();
    }

private:
    GameEngine::Scene& scene;
    bool isVisible = false;

    struct GeneratorFields
    {
        wxChoice* gameObjectIdCtrl{nullptr};
        wxTextCtrl* biasCtrl{nullptr};
        wxTextCtrl* octavesCtrl{nullptr};
        wxComboBox* widthCtrl{nullptr};
    };

    struct HeightPainterFields
    {
        wxComboBox* interpolation{nullptr};
        wxComboBox* brushType{nullptr};
        wxTextCtrl* brushSize{nullptr};
        wxTextCtrl* strength{nullptr};
        std::optional<Utils::ImageData> snapshot;
    };

    struct TexturePainterFields
    {
        wxComboBox* interpolation{nullptr};
        wxComboBox* brushType{nullptr};
        wxTextCtrl* brushSize{nullptr};
        wxTextCtrl* strength{nullptr};

        TextureButton* selectedTextureButton;
        std::optional<GameEngine::File> selectedTextureFile;

        std::vector<GameEngine::File> textures;
        std::unordered_map<GameEngine::Components::TerrainRendererComponent*, TextureCommandEntry> textureCommandEntriesMap;
    };

    struct PlantPainterFields
    {
        wxComboBox* mode{nullptr};
        wxComboBox* brushType{nullptr};
        wxTextCtrl* brushSize{nullptr};
        wxTextCtrl* density{nullptr};
        wxTextCtrl* randomness{nullptr};
        wxTextCtrl* sizeRandomness{nullptr};
        ColorPickerResult* baseColor{nullptr};

        std::optional<GameEngine::File> selectedTextureFile;
    };

    struct PainterFields
    {
        wxComboBox* painterTypeCtrl;
        wxButton* enableDisablePainterButton{nullptr};
        std::unique_ptr<GameEngine::Painter> terrainPainter_;
        HeightPainterFields heightPainterFields;
        TexturePainterFields texturePainterFields;
        PlantPainterFields plantPainterFields;
    };

    PainterFields painterFields;
    GeneratorFields generatorFields;
    std::optional<IdType> sceneEventSubId;
    std::optional<IdType> escapeSubId;

    GameEngine::GameObject* visualizationObject{nullptr};

    wxDECLARE_EVENT_TABLE();
};
