#pragma once
#include <wx/wx.h>

class AddElementDialog : public wxDialog
{
public:
    AddElementDialog(wxWindow*, const std::vector<std::string>&);
    std::string GetSelectedLayer();

private:
    wxComboBox* layerCombo;
};
