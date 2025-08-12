#pragma once

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
    using SelectCallback = std::function<void(const GameEngine::Components::IComponent&)>;

    ComponentPickerPopup(wxWindow*, GameEngine::GameObject&, SelectCallback);

private:
    GameEngine::GameObject& gameObject;
    wxTextCtrl* searchCtrl;
    wxListBox* listBox;
    SelectCallback selectCallback;

    void PopulateList(const wxString& filter);
    void OnSearch(wxCommandEvent& evt);
    void OnSelect(wxCommandEvent& evt);  // nadpisujemy event listboxa
    void OnDismiss() override;           // opcjonalnie, jeśli chcesz reagować na zamknięcie

    wxDECLARE_EVENT_TABLE();
};
