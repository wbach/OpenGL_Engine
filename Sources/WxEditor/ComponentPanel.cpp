#include "ComponentPanel.h"

#include <GameEngine/Components/IComponent.h>
#include <Utils/TreeNode.h>
#include <wx/spinctrl.h>

namespace GameEngine
{
namespace Components
{
extern const std::string CSTR_TYPE;
}
}  // namespace GameEngine

ComponentPanel::ComponentPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)  // zmiana z wxScrolledWindow na wxPanel
{
    mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);
}

void ComponentPanel::ClearComponents()
{
    mainSizer->Clear(true);
    Layout();
}

void ComponentPanel::AddComponent(GameEngine::Components::IComponent& component)
{
    TreeNode node("component");
    component.write(node);
    auto typeName = node.getAttributeValue(GameEngine::Components::CSTR_TYPE);

    wxCollapsiblePane* collapsible = new wxCollapsiblePane(this, wxID_ANY, typeName);
    wxWindow* pane                 = collapsible->GetPane();
    CreateUIForComponent(component, pane);
    mainSizer->Add(collapsible, 0, wxEXPAND | wxALL, 0);

    collapsible->Collapse(false);

    // Podpięcie eventu do aktualizacji layoutu po zwinięciu/rozwinieciu
    collapsible->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED,
                      [this](wxCollapsiblePaneEvent& evt)
                      {
                          this->Layout();
                          this->FitInside();
                          if (GetParent())
                          {
                              GetParent()->Layout();
                          }
                          evt.Skip();
                      });

    this->Layout();
    this->FitInside();
}

void ComponentPanel::CreateUIForComponentExample(const GameEngine::Components::IComponent& component, wxWindow* pane)
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

        browseBtn->Bind(wxEVT_BUTTON,
                        [fileCtrl, pane](wxCommandEvent&)
                        {
                            wxFileDialog openFileDialog(pane, "Choose file", "", "", "*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                            if (openFileDialog.ShowModal() == wxID_OK)
                            {
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

        browseBtn->Bind(wxEVT_BUTTON,
                        [textureCtrl, preview, pane](wxCommandEvent&)
                        {
                            wxFileDialog openFileDialog(pane, "Choose texture", "", "",
                                                        "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                            if (openFileDialog.ShowModal() == wxID_OK)
                            {
                                wxString path = openFileDialog.GetPath();
                                textureCtrl->SetValue(path);

                                wxImage img;
                                if (img.LoadFile(path))
                                {
                                    wxBitmap bmp(img.Scale(64, 64));
                                    preview->SetBitmap(bmp);
                                    pane->Layout();
                                }
                                else
                                {
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

void ComponentPanel::CreateUIForComponent(GameEngine::Components::IComponent& component, wxWindow* pane)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    pane->SetSizer(sizer);

    // Checkbox aktywności
    {
        wxCheckBox* activeCheck = new wxCheckBox(pane, wxID_ANY, "Active");
        activeCheck->SetValue(component.IsActive());
        sizer->Add(activeCheck, 0, wxALL, 5);
    }

    for (auto& field : component.GetFields())
    {
        switch (field.type)
        {
            case GameEngine::Components::FieldType::Int:
            {
                auto val        = static_cast<int*>(field.ptr);
                wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);

                // Etykieta z nazwą pola
                row->Add(new wxStaticText(pane, wxID_ANY, field.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

                // SpinCtrl dla liczb całkowitych
                wxSpinCtrl* intCtrl = new wxSpinCtrl(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
                intCtrl->SetRange(-1000, 1000);
                intCtrl->SetValue(*val);

                row->Add(intCtrl, 1);
                sizer->Add(row, 0, wxEXPAND | wxALL, 5);
            }
            break;
            case GameEngine::Components::FieldType::Float:
            {
                auto val        = static_cast<float*>(field.ptr);
                wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
                row->Add(new wxStaticText(pane, wxID_ANY, field.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
                wxSpinCtrlDouble* floatCtrl =
                    new wxSpinCtrlDouble(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
                floatCtrl->SetRange(-1000.0, 1000.0);
                floatCtrl->SetValue(*val);
                floatCtrl->SetIncrement(0.01);
                floatCtrl->SetDigits(2);
                row->Add(floatCtrl, 1);
                sizer->Add(row, 0, wxEXPAND | wxALL, 5);
            }
            break;

            case GameEngine::Components::FieldType::String:
            {
                auto val        = static_cast<std::string*>(field.ptr);
                wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
                row->Add(new wxStaticText(pane, wxID_ANY, field.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
                wxTextCtrl* stringCtrl = new wxTextCtrl(pane, wxID_ANY, *val);
                row->Add(stringCtrl, 1);
                sizer->Add(row, 0, wxEXPAND | wxALL, 5);
            }
            break;
        case GameEngine::Components::FieldType::VectorOfStrings:
        {
            auto val = static_cast<std::vector<std::string>*>(field.ptr);

            wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);

            // Lewa kolumna: nazwa pola (np. "names")
            wxStaticText* nameLabel = new wxStaticText(pane, wxID_ANY, field.name);
            row->Add(nameLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

            // Prawa kolumna: pionowy sizer z "size" i elementami
            wxBoxSizer* valuesSizer = new wxBoxSizer(wxVERTICAL);

            // Nowy wiersz: label "size" + wartość size obok siebie poziomo
            wxBoxSizer* sizeRow = new wxBoxSizer(wxHORIZONTAL);
            wxStaticText* sizeLabel = new wxStaticText(pane, wxID_ANY, "Vector size:");
            wxTextCtrl* sizeCtrl = new wxTextCtrl(pane, wxID_ANY,
                                                  wxString::Format("%zu", val->size()),
                                                  wxDefaultPosition, wxDefaultSize,
                                                  wxTE_READONLY);
            sizeRow->Add(sizeLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
            sizeRow->Add(sizeCtrl, 0, wxALIGN_CENTER_VERTICAL);

            valuesSizer->Add(sizeRow, 0, wxBOTTOM, 10);

            // Kontrolki dla każdego stringa (poniżej sizeRow)
            for (const auto& str : *val)
            {
                wxTextCtrl* stringCtrl = new wxTextCtrl(pane, wxID_ANY, str);
                valuesSizer->Add(stringCtrl, 0, wxEXPAND | wxBOTTOM, 3);

                // Tu możesz dodać bind do aktualizacji vectora, jeśli chcesz
            }

            row->Add(valuesSizer, 1, wxEXPAND);

            sizer->Add(row, 0, wxEXPAND | wxALL, 5);

            // Jeśli później będziesz dynamicznie zmieniać vector,
            // pamiętaj aktualizować sizeCtrl->SetValue(...)
        }
        break;
            case GameEngine::Components::FieldType::Bool:
            {
                bool* val         = static_cast<bool*>(field.ptr);
                wxCheckBox* check = new wxCheckBox(pane, wxID_ANY, field.name);
                check->SetValue(*val);
                check->SetValue(component.IsActive());
                check->Bind(wxEVT_CHECKBOX, [val](wxCommandEvent& e) { *val = e.IsChecked(); });
                sizer->Add(check, 0, wxALL, 5);

                //                    bool* val  = static_cast<bool*>(field.ptr);
                //                    auto check = new wxCheckBox(pane, wxID_ANY, field.name);
                //                    check->SetValue(*val);
                //                    check->Bind(wxEVT_CHECKBOX, [val](wxCommandEvent& e) { *val = e.IsChecked(); });
                //                    sizer->Add(check, 0, wxEXPAND | wxALL, 5);
            }
            break;
        }
    }

    sizer->Layout();
    pane->Layout();
}
