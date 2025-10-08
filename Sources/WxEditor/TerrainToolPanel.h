#pragma once

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/wx.h>

namespace GameEngine
{
class Scene;
}

class TerrainToolPanel : public wxScrolledWindow
{
public:
    TerrainToolPanel(wxWindow* parent, GameEngine::Scene&, int width);
    ~TerrainToolPanel() override = default;

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

private:
    GameEngine::Scene& scene;
    bool isVisible = false;

    struct GeneratorFields
    {
        wxTextCtrl* gameObjectIdCtrl{nullptr};
        wxTextCtrl* biasCtrl{nullptr};
        wxTextCtrl* octavesCtrl{nullptr};
        wxComboBox* widthCtrl{nullptr};
    };

    GeneratorFields generatorFields;

    wxDECLARE_EVENT_TABLE();
};
