#pragma once

#include <GameEngine/Components/ComponentController.h>
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

class ComponentPickerPopup : public wxDialog
{
public:
    using SelectCallback = std::function<void(GameEngine::Components::IComponent&)>;

    ComponentPickerPopup(wxWindow* parent, GameEngine::Components::ComponentController& componentController,
                         GameEngine::GameObject& gameObject, SelectCallback onSelect);

    // Pokazuje popup w wybranej pozycji (zachowuje stare wywo³anie Popup)
    void Popup(const wxPoint& pos);


private:
    GameEngine::Components::ComponentController& componentController;
    GameEngine::GameObject& gameObject;
    wxTextCtrl* searchCtrl;
    wxListBox* listBox;
    SelectCallback selectCallback;

    void PopulateList(const wxString& filter);
    void OnSearch(wxCommandEvent& evt);
    void OnSelect(wxCommandEvent& evt);

    void Dismiss();  // zamkniêcie popupu
};
