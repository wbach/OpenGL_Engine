#pragma once
#include <GameEngine/Components/ComponentController.h>
#include <GameEngine/Components/IComponent.h>
#include <GameEngine/Components/ReadAnimationInfo.h>
#include <GameEngine/Resources/File.h>
#include <wx/collpane.h>
#include <wx/wx.h>

#include <functional>
#include <vector>

class ComponentPanel : public wxPanel
{
public:
    explicit ComponentPanel(wxWindow*, GameEngine::Components::ComponentController&, IdType);

    void ClearComponents();
    void AddComponent(GameEngine::Components::IComponent&);

private:
    GameEngine::Components::ComponentController& componentController;
    IdType gameObjectId;
    wxBoxSizer* mainSizer{nullptr};
    wxCollapsiblePane* collapsible{nullptr};

    void CreateUIForComponent(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*);
    void CreateUIForField(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);

    template <typename VecT, typename CtrlT, typename BindEvt, typename SetVal, typename GetVal>
    void CreateVectorRow(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const wxString&, VecT*, int, SetVal, GetVal,
                         BindEvt);

    template <typename T>
    void CreateUIForVector(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&,
                           std::function<wxBoxSizer*(wxWindow*, std::vector<T>*, size_t, std::function<void()>)>);

    wxBoxSizer* CreateStringItem(GameEngine::Components::IComponent&, wxWindow*, std::vector<std::string>*, size_t,
                                 std::function<void()>);
    wxBoxSizer* CreateIntItem(GameEngine::Components::IComponent&, wxWindow*, std::vector<int>*, size_t, std::function<void()>);
    wxBoxSizer* CreateFloatItem(GameEngine::Components::IComponent&, wxWindow*, std::vector<float>*, size_t,
                                std::function<void()>);
    wxBoxSizer* CreateFileItem(GameEngine::Components::IComponent&, wxWindow*, std::vector<GameEngine::File>*, size_t,
                               std::function<void()>);
    wxBoxSizer* CreateTextureItem(GameEngine::Components::IComponent&, wxWindow*, std::vector<GameEngine::File>*, size_t,
                                  std::function<void()>);
    wxBoxSizer* CreateAnimationClipItem(GameEngine::Components::IComponent&, wxWindow*,
                                        std::vector<GameEngine::Components::ReadAnimationInfo>*, size_t, std::function<void()>);

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
    };
    BrowseRow CreateBrowseFileRow(wxWindow* parent, const wxString& label, const wxString& initial);

    struct TextureRow
    {
        wxBoxSizer* row;
        wxTextCtrl* textCtrl;
        wxButton* browseBtn;
        wxStaticBitmap* preview;
    };
    TextureRow CreateBrowseTextureRow(wxWindow* parent, const wxString& label, const wxString& initial);
    void SetPreviewBitmap(wxStaticBitmap* preview, const wxString& path, wxWindow* relayoutParent);

    wxBoxSizer* CreateUIForAnimationClip(GameEngine::Components::IComponent&, wxWindow*,
                                         GameEngine::Components::ReadAnimationInfo*);
};
