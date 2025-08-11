#pragma once
#include <wx/collpane.h>
#include <wx/wx.h>

#include <vector>

namespace GameEngine
{
namespace Components
{
class IComponent;
}
}  // namespace GameEngine

class ComponentPanel : public wxPanel
{
public:
    explicit ComponentPanel(wxWindow* parent);

    void ClearComponents();

    // Dodaje UI dla komponentu
    void AddComponent(const GameEngine::Components::IComponent&);

private:
    wxBoxSizer* mainSizer;

    // Pomocnicza funkcja do tworzenia UI dla pojedynczego komponentu
    void CreateUIForComponent(const GameEngine::Components::IComponent&, wxWindow*);
};
