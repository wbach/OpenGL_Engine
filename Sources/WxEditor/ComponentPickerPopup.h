#pragma once

#include <GameEngine/Components/ComponentController.h>
#include <wx/popupwin.h>
#include <wx/wx.h>

#include <functional>
#include <vector>

namespace GameEngine
{
class GameObject;
namespace Components
{
class IComponent;
}
}  // namespace GameEngine

class ComponentPickerPopup : public wxPopupTransientWindow
{
public:
    using SelectCallback = std::function<void(GameEngine::Components::IComponent&)>;

    ComponentPickerPopup(wxWindow*, GameEngine::Components::ComponentController&, GameEngine::GameObject&, SelectCallback);

private:
    GameEngine::Components::ComponentController& componentController;
    GameEngine::GameObject& gameObject;
    wxTextCtrl* searchCtrl;
    wxListBox* listBox;
    SelectCallback selectCallback;

    void PopulateList(const wxString& filter);
    void OnSearch(wxCommandEvent& evt);
    void OnSelect(wxCommandEvent& evt);  // nadpisujemy event listboxa
    void OnDismiss() override;           // opcjonalnie, jesli chcesz reagowac na zamkniecie

    wxDECLARE_EVENT_TABLE();
};
