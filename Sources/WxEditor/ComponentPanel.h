#pragma once
#include <GameEngine/Components/IComponent.h>
#include <wx/collpane.h>
#include <wx/wx.h>

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
};
