#pragma once
#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Resources/Models/Material.h>
#include <wx/colordlg.h>
#include <wx/splitter.h>
#include <wx/wx.h>

class GLCanvas;
class MaterialPanel;

class MaterialEditorFrame : public wxFrame
{
public:
    MaterialEditorFrame(const std::optional<GameEngine::File>&, const wxString& title, const wxPoint& pos, const wxSize& size);

    bool IsMaterialLoaded() const;
    const GameEngine::Material& GetMaterial() const;

private:
    void Init();
    void CreateMainMenu();
    void LoadMaterial(const std::string& file);
    void OnOpenMaterial(wxCommandEvent&);
    void OnSaveMaterial(wxCommandEvent&);
    void OnExit(wxCommandEvent&);
    void OnTimer(wxTimerEvent&);
    void UpdateMaterialInComponent();

    vec3 PickColor(const Color&);
    void OnRoughnessChanged(wxCommandEvent&);

    wxButton* AddColorPicker(wxPanel*, wxBoxSizer*, const wxString&, std::function<void(wxCommandEvent&)>);
    wxBoxSizer* AddTexturePicker(wxPanel*, wxBoxSizer*, const wxString&, wxTextCtrl*&, std::function<void(const std::string&)>);

private:
    wxSplitterWindow* mainSplitter{nullptr};
    GLCanvas* canvas = nullptr;
    wxTimer* timer   = nullptr;

    // Kontrolki GUI
    wxTextCtrl* diffusePathCtrl      = nullptr;
    wxTextCtrl* ambientPathCtrl      = nullptr;
    wxTextCtrl* specularPathCtrl     = nullptr;
    wxTextCtrl* normalPathCtrl       = nullptr;
    wxTextCtrl* displacementPathCtrl = nullptr;
    wxSlider* reflectivitySlider     = nullptr;
    wxSlider* iorSlider              = nullptr;
    wxCheckBox* transparencyCheck    = nullptr;
    wxCheckBox* fakeLightingCheck    = nullptr;

    wxSlider* roughnessSlider = nullptr;
    wxButton* ambientBtn      = nullptr;
    wxButton* diffuseBtn      = nullptr;
    wxButton* specularBtn     = nullptr;

    // Obiekt materiału edytowany w GUI
    GameEngine::Material currentMaterial;
    GameEngine::GameObject* previwGameObject{nullptr};

    bool materialLoaded = false;
};