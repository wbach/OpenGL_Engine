#include "ComponentPanel.h"
#include <GameEngine/Components/IComponent.h>
#include <wx/spinctrl.h>
#include <Utils/TreeNode.h>

namespace GameEngine
{
namespace Components
{
extern const std::string CSTR_TYPE;
}
}  // namespace GameEngine


ComponentPanel::ComponentPanel(wxWindow* parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
{
    SetScrollRate(5, 5);
   // SetMinSize(wxSize(300, 200));
    mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);
}

void ComponentPanel::ClearComponents()
{
    mainSizer->Clear(true);
    Layout();
    FitInside();
}

void ComponentPanel::AddComponent(const GameEngine::Components::IComponent &component)
{
    TreeNode node("component");
    component.write(node);
    auto typeName = node.getAttributeValue(GameEngine::Components::CSTR_TYPE);

    //wxString typeName = component.GetTypeName(); // Załóżmy, że masz taką funkcję

    wxCollapsiblePane* collapsible = new wxCollapsiblePane(this, wxID_ANY, typeName);


    wxWindow* pane = collapsible->GetPane();

    CreateUIForComponent(component, pane);

    mainSizer->Add(collapsible, 0, wxEXPAND | wxALL, 5);

    collapsible->Collapse(false);  // rozwiń panel
    collapsible->GetPane()->Layout();
    collapsible->Layout();
    this->Layout();
    this->FitInside();
}

void ComponentPanel::CreateUIForComponent(const GameEngine::Components::IComponent& component, wxWindow* pane)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Checkbox aktywności
    {
        wxCheckBox* activeCheck = new wxCheckBox(pane, wxID_ANY, "Active");
        activeCheck->SetValue(component.IsActive());
        sizer->Add(activeCheck, 0, wxALL, 5);
    }

    // FLOAT
    {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(pane, wxID_ANY, "Float Value:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        wxSpinCtrlDouble* floatCtrl = new wxSpinCtrlDouble(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
        floatCtrl->SetRange(-1000.0, 1000.0);
        floatCtrl->SetValue(1.23);
        floatCtrl->SetIncrement(0.01);
        floatCtrl->SetDigits(2);
        row->Add(floatCtrl, 1);
        sizer->Add(row, 0, wxEXPAND | wxALL, 5);
    }

    // STRING
    {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(pane, wxID_ANY, "String Value:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        wxTextCtrl* stringCtrl = new wxTextCtrl(pane, wxID_ANY, "Hello World");
        row->Add(stringCtrl, 1);
        sizer->Add(row, 0, wxEXPAND | wxALL, 5);
    }

    // FILE
    {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(pane, wxID_ANY, "File Path:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        wxTextCtrl* fileCtrl = new wxTextCtrl(pane, wxID_ANY, "/path/to/file.txt");
        row->Add(fileCtrl, 1);
        wxButton* browseBtn = new wxButton(pane, wxID_ANY, "Browse");
        row->Add(browseBtn, 0, wxLEFT, 5);

        browseBtn->Bind(wxEVT_BUTTON, [fileCtrl, pane](wxCommandEvent&) {
            wxFileDialog openFileDialog(pane, "Choose file", "", "", "*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (openFileDialog.ShowModal() == wxID_OK) {
                fileCtrl->SetValue(openFileDialog.GetPath());
            }
        });

        sizer->Add(row, 0, wxEXPAND | wxALL, 5);
    }

    // TEXTURE (plik + podgląd)
    {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(pane, wxID_ANY, "Texture:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        auto* textureCtrl = new wxTextCtrl(pane, wxID_ANY, "texture.png");
        row->Add(textureCtrl, 1, wxALIGN_CENTER_VERTICAL);
        wxButton* browseBtn = new wxButton(pane, wxID_ANY, "Browse");
        row->Add(browseBtn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

        wxStaticBitmap* preview = new wxStaticBitmap(pane, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(64, 64));
        row->Add(preview, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 10);

        browseBtn->Bind(wxEVT_BUTTON, [textureCtrl, preview, pane](wxCommandEvent&) {
            wxFileDialog openFileDialog(pane, "Choose texture", "", "",
                "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (openFileDialog.ShowModal() == wxID_OK) {
                wxString path = openFileDialog.GetPath();
                textureCtrl->SetValue(path);

                wxImage img;
                if (img.LoadFile(path)) {
                    wxBitmap bmp(img.Scale(64, 64));
                    preview->SetBitmap(bmp);
                    pane->Layout();
                } else {
                    wxMessageBox("Failed to load image.", "Error", wxICON_ERROR);
                }
            }
        });

        sizer->Add(row, 0, wxEXPAND | wxALL, 5);
    }

    pane->SetSizer(sizer);
    sizer->Layout();
    pane->Layout();
}


