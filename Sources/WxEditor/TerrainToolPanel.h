#pragma once

#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Resources/File.h>
#include <GameEngine/Scene/SceneEvents.h>
#include <Types.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/wx.h>
#include <optional>

namespace GameEngine
{
class Scene;
class Painter;
}  // namespace GameEngine

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
    void BuildTexturePainterPanel(wxWindow* panel, wxSizer* sizer);
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
    void OnUpdatePainterParam();
    void SelectedPainterTexture(wxMouseEvent&);
    GameEngine::GameObject* createPainterVisualizationObject();

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
    };

    struct PainterFields
    {
        wxComboBox* painterTypeCtrl;
        wxButton* enableDisablePainterButton{nullptr};
        std::unique_ptr<GameEngine::Painter> terrainPainter_;
        HeightPainterFields heightPainterFields;
        TexturePainterFields texturePainterFields;
    };

    PainterFields painterFields;
    GeneratorFields generatorFields;
    std::optional<IdType> sceneEventSubId;

    GameEngine::GameObject* visualizationObject{nullptr};

    wxDECLARE_EVENT_TABLE();
};
