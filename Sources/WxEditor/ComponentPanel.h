#pragma once
#include <GameEngine/Components/IComponent.h>
#include <wx/collpane.h>
#include <wx/wx.h>

#include <functional>
#include <vector>

class ComponentPanel : public wxPanel
{
public:
    explicit ComponentPanel(wxWindow* parent);

    void ClearComponents();

    // Dodaje UI dla komponentu
    void AddComponent(GameEngine::Components::IComponent&);

private:
    wxBoxSizer* mainSizer;
    wxCollapsiblePane* collapsible;

    void CreateUIForComponentExample(const GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*);
    void CreateUIForComponent(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*);

    void CreateUIForInt(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);
    void CreateUIForFloat(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&);
    void CreateUIForString(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                           const GameEngine::Components::FieldInfo& field);
    void CreateUIForBool(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                         const GameEngine::Components::FieldInfo& field);
    void CreateUIForVectorOfStrings(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                                    const GameEngine::Components::FieldInfo&);
    void CreateUIForVectorOfInts(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                                    const GameEngine::Components::FieldInfo&);
    void CreateUIForVectorOfFloats(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*,
                                    const GameEngine::Components::FieldInfo&);

    template <typename T>
    void CreateUIForVector(GameEngine::Components::IComponent&, wxWindow*, wxBoxSizer*, const GameEngine::Components::FieldInfo&,
                           std::function<wxBoxSizer*(wxWindow*, std::vector<T>*, size_t, std::function<void()>)>);

    wxBoxSizer* CreateStringControl(wxWindow*, std::vector<std::string>*, size_t, std::function<void()>);
    wxBoxSizer* CreateIntControl(wxWindow*, std::vector<int>*, size_t, std::function<void()>);
    wxBoxSizer* CreateFloatControl(wxWindow*, std::vector<float>*, size_t, std::function<void()>);
};
