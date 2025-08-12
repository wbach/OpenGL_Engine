#pragma once

#include <wx/wx.h>
#include <wx/popupwin.h>
#include <functional>
#include <vector>

class ComponentPickerPopup : public wxPopupTransientWindow
{
public:
    using SelectCallback = std::function<void(const wxString&)>;

    ComponentPickerPopup(wxWindow* parent,
                         const std::vector<wxString>& components,
                         SelectCallback onSelect);

private:
    wxTextCtrl* searchCtrl;
    wxListBox* listBox;
    std::vector<wxString> allComponents;
    SelectCallback selectCallback;

    void PopulateList(const wxString& filter);
    void OnSearch(wxCommandEvent& evt);
    void OnSelect(wxCommandEvent& evt); // nadpisujemy event listboxa
    void OnDismiss() override; // opcjonalnie, jeśli chcesz reagować na zamknięcie

    wxDECLARE_EVENT_TABLE();
};
