#pragma once
#include <GameEngine/Components/ComponentController.h>
#include <GameEngine/Components/IComponent.h>
#include <GameEngine/Components/VectorOfCustomStructure.h>
#include <GameEngine/Components/ReadAnimationInfo.h>
#include <GameEngine/Components/Renderer/Terrain/TerrainTexture.h>
#include <GameEngine/Resources/File.h>
#include <GameEngine/Resources/Models/Material.h>
#include <wx/collpane.h>
#include <wx/spinctrl.h>
#include <wx/wx.h>

#include <functional>
#include <mutex>
#include <vector>

namespace GameEngine
{
class GameObject;
struct ExternalComponentsReader;
}  // namespace GameEngine

class ComponentPanel : public wxPanel
{
public:
    explicit ComponentPanel(wxFrame*, wxWindow*, GameEngine::ExternalComponentsReader&, GameEngine::GameObject&);

    void ClearComponents();
    void AddComponent(GameEngine::Components::IComponent&, bool collapsed = true);
    void Unlock();
    void Lock();

private:
    void CreateUIForComponent(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*);
    void CreateUIForField(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);

    template <typename VecT, typename CtrlT, typename BindEvt, typename SetVal, typename GetVal>
    void CreateVectorRow(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const wxString&, VecT*, int, SetVal, GetVal,
                         BindEvt, const std::vector<std::string>& = {"X:", "Y:", "Z:", "W:"});

    template <typename T, typename Container = std::vector<T>>
    void CreateUIForVector(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&,
                           std::function<wxBoxSizer*(wxWindow*, T&)>, bool resizeable = true);

    wxBoxSizer* createItem(GameEngine::Components::IComponent&, wxWindow*, std::string&);
    wxBoxSizer* createItem(GameEngine::Components::IComponent&, wxWindow*, int&);
    wxBoxSizer* createItem(GameEngine::Components::IComponent&, wxWindow*, float&);
    wxBoxSizer* createItem(GameEngine::Components::IComponent&, wxWindow*, vec3&);
    wxBoxSizer* createItem(GameEngine::Components::IComponent&, wxWindow*, GameEngine::File&);
    wxBoxSizer* createTextureItem(GameEngine::Components::IComponent&, wxWindow*, GameEngine::File&);
    wxBoxSizer* createItem(GameEngine::Components::IComponent&, wxWindow*, GameEngine::Components::ReadAnimationInfo&);
    wxBoxSizer* createItem(GameEngine::Components::IComponent&, wxWindow*, GameEngine::Components::TerrainTexture&);
    wxBoxSizer* createItem(GameEngine::Components::IComponent&, wxWindow*, GameEngine::Components::CustomStructure&);
    // == Akcje/logika ==
    void reInitComponent(GameEngine::Components::IComponent&);
    void browseFileControlAction(wxCommandEvent&, GameEngine::Components::IComponent& component, wxTextCtrl* fileCtrl,
                                 wxWindow* pane, GameEngine::File* val);

    wxBoxSizer* CreateLabeledRow(wxWindow* parent, const wxString& label, wxWindow* control, int proportion = 1,
                                 int flags = wxEXPAND | wxRIGHT, int border = 5);

    wxTextCtrl* createTextEnterCtrl(wxWindow* pane, const std::string& text);

    struct BrowseRow
    {
        wxBoxSizer* row;
        wxTextCtrl* textCtrl;
        wxButton* browseBtn;
        wxStaticBitmap* warningIcon;
    };
    BrowseRow CreateBrowseFileRow(wxWindow* parent, const wxString& label, const wxString& initial);

    struct TextureRow
    {
        wxBoxSizer* row;
        wxTextCtrl* textCtrl;
        wxButton* browseBtn;
        wxStaticBitmap* preview;
        wxStaticBitmap* warningIcon;
    };
    TextureRow CreateBrowseTextureRow(wxWindow* parent, const wxString& label, const wxString& initial);

    void SetPreviewBitmap(wxStaticBitmap*, const GameEngine::File&, wxWindow*);
    void SetPreviewBitmap(wxStaticBitmap* preview, const wxString& path, wxWindow* relayoutParent);

    wxBoxSizer* CreateUIForAnimationClip(GameEngine::Components::IComponent&, wxWindow*,
                                         GameEngine::Components::ReadAnimationInfo*);
    wxSpinCtrlDouble* CreateFloatSpinCtrl(wxWindow* parent, double, double = -100000.0, double = 100000.0, double = 0.01,
                                          int = 3);

    void UpdateFileWarning(wxStaticBitmap* warningIcon, const GameEngine::File& file);

    void CreateUIForMaterialsMap(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, GameEngine::MaterialsMap&);

private:
    std::mutex mutex;
    wxFrame* mainFrame;
    GameEngine::ExternalComponentsReader& externalComponentsReader;
    GameEngine::GameObject& gameObject;
    wxBoxSizer* mainSizer{nullptr};
    wxCollapsiblePane* collapsible{nullptr};
    wxPanel* headerPanel{nullptr};
};
