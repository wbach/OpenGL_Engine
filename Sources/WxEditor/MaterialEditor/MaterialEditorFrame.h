#pragma once

#include <GameEngine/Resources/File.h>
#include <GameEngine/Resources/Models/Material.h>
#include <wx/colordlg.h>
#include <wx/filedlg.h>
#include <wx/splitter.h>
#include <wx/wx.h>

#include <optional>

class GLCanvas;

namespace GameEngine
{
class GameObject;
}

class MaterialEditorFrame : public wxFrame
{
public:
    MaterialEditorFrame(const std::optional<GameEngine::File>& maybeFile, const wxString& title, const wxPoint& pos,
                        const wxSize& size);

    void LoadMaterial(const std::string& file);
    bool IsMaterialLoaded() const;
    const GameEngine::Material& GetMaterial() const;

private:
    void Init();
    void CreateMainMenu();
    void UpdateMaterialInComponent();

    // Event Handlers
    void OnTimer(wxTimerEvent& event);
    void OnOpenMaterial(wxCommandEvent& event);
    void OnSaveMaterial(wxCommandEvent& event);
    void OnSaveMaterialAs(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    // Helpers dla UI
    wxButton* AddColorPicker(wxPanel* parent, wxBoxSizer* sizer, const wxString& labelText,
                             std::function<void(wxCommandEvent&)> onChange);

    wxBoxSizer* AddTexturePicker(wxPanel* parent, wxBoxSizer* sizer, const wxString& labelText, wxTextCtrl*& outTextCtrl,
                                 std::function<void(const std::string&)> onTextureChanged);

    vec3 PickColor(const Color& currentColor);

private:
    std::optional<GameEngine::File> initialFile;
    std::filesystem::path lastDirPath;

    // Engine Core
    GLCanvas* canvas               = nullptr;
    wxSplitterWindow* mainSplitter = nullptr;
    wxTimer* timer                 = nullptr;

    GameEngine::Material currentMaterial;
    GameEngine::GameObject* previwGameObject{nullptr};
    bool materialLoaded = false;

    // UI Controls - Kolory
    wxButton* baseColorBtn       = nullptr;
    wxButton* subsurfaceColorBtn = nullptr;

    // UI Controls - Slidery PBR
    wxSlider* metallicSlider      = nullptr;
    wxSlider* roughnessSlider     = nullptr;
    wxSlider* occlusionSlider     = nullptr;
    wxSlider* normalScaleSlider   = nullptr;
    wxSlider* iorSlider           = nullptr;
    wxSlider* opacityCutoffSlider = nullptr;
    wxSlider* sssStrengthSlider   = nullptr;
    wxSlider* uvScaleSlider       = nullptr;

    // UI Controls - Flagi
    wxCheckBox* doubleSidedCheck = nullptr;
    wxCheckBox* alphaTestCheck   = nullptr;
    wxCheckBox* alphaBlendCheck  = nullptr;
    wxCheckBox* foliageCheck     = nullptr;
    wxCheckBox* unlitCheck       = nullptr;
    wxCheckBox* shadowCheck      = nullptr;

    // UI Controls - Tekstury (Ścieżki)
    wxTextCtrl* baseColorPathCtrl    = nullptr;
    wxTextCtrl* normalPathCtrl       = nullptr;
    wxTextCtrl* metallicPathCtrl     = nullptr;
    wxTextCtrl* roughnessPathCtrl    = nullptr;
    wxTextCtrl* occlusionPathCtrl    = nullptr;
    wxTextCtrl* emissionPathCtrl     = nullptr;
    wxTextCtrl* opacityPathCtrl      = nullptr;
    wxTextCtrl* displacementPathCtrl = nullptr;
};