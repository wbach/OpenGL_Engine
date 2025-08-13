#pragma once
#include <GameEngine/Components/ComponentController.h>
#include <GameEngine/Components/IComponent.h>
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

    // Dodaje UI dla komponentu
    void AddComponent(GameEngine::Components::IComponent&);

private:
    GameEngine::Components::ComponentController& componentController;
    IdType gameObjectId;
    wxBoxSizer* mainSizer;
    wxCollapsiblePane* collapsible;

    void CreateUIForComponentExample(const GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*);
    void CreateUIForComponent(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*);

    void CreateUIForUInt(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);
    void CreateUIForInt(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);
    void CreateUIForFloat(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);
    void CreateUIForString(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);
    void CreateUIForBool(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);
    void CreateUIForTexture(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                            const GameEngine::Components::FieldInfo&);
    void CreateUIForFile(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);
    void CreateUIForVectorOfStrings(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                                    const GameEngine::Components::FieldInfo&);
    void CreateUIForVectorOfInts(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                                 const GameEngine::Components::FieldInfo&);
    void CreateUIForVectorOfFloats(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                                   const GameEngine::Components::FieldInfo&);
    void CreateUIForVectorOfTextures(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                                     const GameEngine::Components::FieldInfo&);
    void CreateUIForVectorOfFiles(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                                  const GameEngine::Components::FieldInfo&);

    template <typename T>
    void CreateUIForVector(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&,
                           std::function<wxBoxSizer*(wxWindow*, std::vector<T>*, size_t, std::function<void()>)>);

    wxBoxSizer* CreateStringControl(GameEngine::Components::IComponent&, wxWindow*, std::vector<std::string>*, size_t,
                                    std::function<void()>);
    wxBoxSizer* CreateIntControl(GameEngine::Components::IComponent&, wxWindow*, std::vector<int>*, size_t,
                                 std::function<void()>);
    wxBoxSizer* CreateFloatControl(GameEngine::Components::IComponent&, wxWindow*, std::vector<float>*, size_t,
                                   std::function<void()>);
    wxBoxSizer* CreateFileControl(GameEngine::Components::IComponent&, wxWindow*, std::vector<GameEngine::File>*, size_t,
                                  std::function<void()>);
    wxBoxSizer* CreateTextureControl(GameEngine::Components::IComponent&, wxWindow*, std::vector<GameEngine::File>*, size_t,
                                     std::function<void()>);

    void reInitComponent(GameEngine::Components::IComponent&);

    void browseFileControlAction(wxCommandEvent&, GameEngine::Components::IComponent& component, wxTextCtrl* fileCtrl,
                                 wxWindow* pane, GameEngine::File* val);

    wxTextCtrl* createTextEnterCtrl(wxWindow* pane, const std::string& text)
    {
        return new wxTextCtrl(pane, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    }
};
