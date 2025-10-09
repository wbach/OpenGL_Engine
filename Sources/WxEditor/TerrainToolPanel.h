#pragma once

#include <GameEngine/Scene/SceneEvents.h>
#include <Types.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/wx.h>
#include <set>
#include <unordered_map>
#include "Objects/GameObject.h"

namespace GameEngine
{
class Scene;
}

class TerrainToolPanel : public wxScrolledWindow
{
public:
    TerrainToolPanel(wxWindow* parent, GameEngine::Scene&, int width);
    ~TerrainToolPanel() override;

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
    void GenerateTerrain(bool);
    void GenerateTerrain(bool, const std::optional<IdType>&);
    GameEngine::GameObject* CreateTerrainObject();
    void CreateAndGenerateTerrain(bool);
    void GenerateTerrainForExistObject(bool, IdType);
    void GenerateForAllTerrains(bool);
    void RefillTerrainObjectsCtrl();
    void DetectedTerrainGameObjectsAndAddToChoice();
    void ImportFromMesh();

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

    GeneratorFields generatorFields;
    std::optional<IdType> sceneEventSubId;

    wxDECLARE_EVENT_TABLE();
};
