#include "ComponentPanel.h"

#include <GameEngine/Engine/Configuration.h>
#include <Logger/Log.h>
#include <Utils/TreeNode.h>
#include <wx/artprov.h>
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

    collapsible    = new wxCollapsiblePane(this, wxID_ANY, typeName);
    wxWindow* pane = collapsible->GetPane();

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    pane->SetSizer(sizer);

    CreateUIForComponent(component, pane, sizer);

    pane->SetSizer(sizer);
    sizer->Layout();
    pane->Layout();

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

void ComponentPanel::CreateUIForComponentExample(const GameEngine::Components::IComponent& component, wxWindow* pane,
                                                 wxBoxSizer* sizer)
{
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

    sizer->Layout();
    pane->Layout();
}

void ComponentPanel::CreateUIForComponent(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer)
{
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
            case GameEngine::Components::FieldType::UInt:
                CreateUIForUInt(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::Int:
                CreateUIForInt(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::Float:
                CreateUIForFloat(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::String:
                CreateUIForString(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::Bool:
                CreateUIForBool(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::Texture:
                CreateUIForTexture(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::File:
                CreateUIForFile(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::VectorOfStrings:
                CreateUIForVectorOfStrings(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::VectorOfInt:
                CreateUIForVectorOfInts(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::VectorOfFloat:
                CreateUIForVectorOfFloats(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::VectorOfFiles:
                CreateUIForVectorOfTextures(component, pane, sizer, field);
                break;
            case GameEngine::Components::FieldType::VectorOfTextures:
                CreateUIForVectorOfFiles(component, pane, sizer, field);
                break;
        }
    }
}

void ComponentPanel::CreateUIForUInt(GameEngine::Components::IComponent& /*component*/, wxWindow* pane, wxBoxSizer* sizer,
                                     const GameEngine::Components::FieldInfo& field)
{
    int* val        = static_cast<int*>(field.ptr);
    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(pane, wxID_ANY, field.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    wxSpinCtrl* intCtrl = new wxSpinCtrl(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
    intCtrl->SetRange(-1000, 1000);
    intCtrl->SetValue(*val);

    row->Add(intCtrl, 1);
    sizer->Add(row, 0, wxEXPAND | wxALL, 5);

    // Możesz dodać obsługę zdarzeń, np.:
    intCtrl->Bind(wxEVT_SPINCTRL, [val](wxSpinEvent& evt) { *val = evt.GetValue(); });
}

void ComponentPanel::CreateUIForInt(GameEngine::Components::IComponent& /*component*/, wxWindow* pane, wxBoxSizer* sizer,
                                    const GameEngine::Components::FieldInfo& field)
{
    int* val        = static_cast<int*>(field.ptr);
    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(pane, wxID_ANY, field.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    wxSpinCtrl* intCtrl = new wxSpinCtrl(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
    intCtrl->SetRange(-1000, 1000);
    intCtrl->SetValue(*val);

    row->Add(intCtrl, 1);
    sizer->Add(row, 0, wxEXPAND | wxALL, 5);

    // Możesz dodać obsługę zdarzeń, np.:
    intCtrl->Bind(wxEVT_SPINCTRL, [val](wxSpinEvent& evt) { *val = evt.GetValue(); });
}

void ComponentPanel::CreateUIForFloat(GameEngine::Components::IComponent& /*component*/, wxWindow* pane, wxBoxSizer* sizer,
                                      const GameEngine::Components::FieldInfo& field)
{
    float* val      = static_cast<float*>(field.ptr);
    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(pane, wxID_ANY, field.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    wxSpinCtrlDouble* floatCtrl = new wxSpinCtrlDouble(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
    floatCtrl->SetRange(-1000.0, 1000.0);
    floatCtrl->SetValue(*val);
    floatCtrl->SetIncrement(0.01);
    floatCtrl->SetDigits(2);

    row->Add(floatCtrl, 1);
    sizer->Add(row, 0, wxEXPAND | wxALL, 5);

    floatCtrl->Bind(wxEVT_SPINCTRLDOUBLE, [val](wxSpinDoubleEvent& evt) { *val = evt.GetValue(); });
}

void ComponentPanel::CreateUIForString(GameEngine::Components::IComponent& /*component*/, wxWindow* pane, wxBoxSizer* sizer,
                                       const GameEngine::Components::FieldInfo& field)
{
    std::string* val = static_cast<std::string*>(field.ptr);
    wxBoxSizer* row  = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(pane, wxID_ANY, field.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    wxTextCtrl* stringCtrl = new wxTextCtrl(pane, wxID_ANY, *val);
    row->Add(stringCtrl, 1);
    sizer->Add(row, 0, wxEXPAND | wxALL, 5);

    stringCtrl->Bind(wxEVT_TEXT, [val](wxCommandEvent& evt) { *val = evt.GetString().ToStdString(); });
}

void ComponentPanel::CreateUIForBool(GameEngine::Components::IComponent& /*component*/, wxWindow* pane, wxBoxSizer* sizer,
                                     const GameEngine::Components::FieldInfo& field)
{
    auto val = static_cast<bool*>(field.ptr);

    wxCheckBox* check = new wxCheckBox(pane, wxID_ANY, field.name);
    check->SetValue(*val);
    sizer->Add(check, 0, wxALL, 5);

    check->Bind(wxEVT_CHECKBOX, [val](wxCommandEvent& e) { *val = e.IsChecked(); });
}

void ComponentPanel::CreateUIForFile(GameEngine::Components::IComponent& /*component*/, wxWindow* pane, wxBoxSizer* sizer,
                                     const GameEngine::Components::FieldInfo& field)
{
    auto val = static_cast<GameEngine::File*>(field.ptr);

    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(pane, wxID_ANY, field.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    wxTextCtrl* fileCtrl = new wxTextCtrl(pane, wxID_ANY, val->GetDataRelativeDir());
    row->Add(fileCtrl, 1);
    wxButton* browseBtn = new wxButton(pane, wxID_ANY, "Browse");
    row->Add(browseBtn, 0, wxLEFT, 5);

    browseBtn->Bind(wxEVT_BUTTON,
                    [fileCtrl, pane, val](wxCommandEvent&)
                    {
                        wxFileDialog openFileDialog(pane, "Choose file", "", "", "*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                        if (openFileDialog.ShowModal() == wxID_OK)
                        {
                            fileCtrl->SetValue(openFileDialog.GetPath());
                            val->Change(openFileDialog.GetPath().ToStdString());
                        }
                    });

    sizer->Add(row, 0, wxEXPAND | wxALL, 5);
}

void ComponentPanel::CreateUIForTexture(GameEngine::Components::IComponent& /*component*/, wxWindow* pane, wxBoxSizer* sizer,
                                        const GameEngine::Components::FieldInfo& field)
{
    auto val = static_cast<GameEngine::File*>(field.ptr);

    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(pane, wxID_ANY, field.name), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    auto* textureCtrl = new wxTextCtrl(pane, wxID_ANY, val->GetDataRelativeDir());
    row->Add(textureCtrl, 1, wxALIGN_CENTER_VERTICAL);
    wxButton* browseBtn = new wxButton(pane, wxID_ANY, "Browse");
    row->Add(browseBtn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    wxStaticBitmap* preview = new wxStaticBitmap(pane, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(64, 64));
    row->Add(preview, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 10);

    browseBtn->Bind(wxEVT_BUTTON,
                    [textureCtrl, preview, pane, val](wxCommandEvent&)
                    {
                        wxFileDialog openFileDialog(pane, "Choose texture", "", "",
                                                    "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                        if (openFileDialog.ShowModal() == wxID_OK)
                        {
                            wxString path = openFileDialog.GetPath();
                            textureCtrl->SetValue(path);
                            val->Change(path.ToStdString());

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

template <typename T>
void ComponentPanel::CreateUIForVector(
    GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
    const GameEngine::Components::FieldInfo& field,
    std::function<wxBoxSizer*(wxWindow*, std::vector<T>*, size_t, std::function<void()>)> createElementControl)
{
    auto val = static_cast<std::vector<T>*>(field.ptr);

    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* nameLabel = new wxStaticText(pane, wxID_ANY, field.name);
    row->Add(nameLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

    wxBoxSizer* valuesSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* sizeRow     = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* sizeLabel = new wxStaticText(pane, wxID_ANY, "Vector size:");
    wxTextCtrl* sizeCtrl = new wxTextCtrl(pane, wxID_ANY, wxString::Format("%zu", val->size()), wxDefaultPosition, wxDefaultSize,
                                          wxTE_PROCESS_ENTER);

    sizeRow->Add(sizeLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    sizeRow->Add(sizeCtrl, 0, wxALIGN_CENTER_VERTICAL);
    valuesSizer->Add(sizeRow, 0, wxBOTTOM, 10);

    auto rebuildUI = [this, &component]()
    {
        mainSizer->Clear(true);
        AddComponent(component);

        if (wxWindow* p = this->GetParent())
        {
            p->Layout();
            p->FitInside();
        }
    };

    sizeCtrl->Bind(wxEVT_TEXT_ENTER,
                   [this, val, sizeCtrl, rebuildUI](auto&)
                   {
                       try
                       {
                           int requestedSize = std::stoi(sizeCtrl->GetValue().ToStdString());
                           if (requestedSize < 0)
                               requestedSize = 0;

                           if (requestedSize != val->size())
                           {
                               val->resize(requestedSize);
                               this->CallAfter(rebuildUI);
                           }
                       }
                       catch (...)
                       {
                           // ignoruj błędy parsowania
                       }
                   });

    for (size_t i = 0; i < val->size(); ++i)
    {
        wxBoxSizer* elemRow = createElementControl(pane, val, i, rebuildUI);
        valuesSizer->Add(elemRow, 0, wxEXPAND | wxBOTTOM, 3);
    }

    wxButton* addButton = new wxButton(pane, wxID_ANY, "Add element");
    valuesSizer->Add(addButton, 0, wxLEFT, 10);

    addButton->Bind(wxEVT_BUTTON,
                    [this, val, rebuildUI](const auto&)
                    {
                        val->push_back({});
                        this->CallAfter(rebuildUI);
                    });

    row->Add(valuesSizer, 1, wxEXPAND);

    sizer->Add(row, 0, wxEXPAND | wxALL, 5);
}

wxBoxSizer* ComponentPanel::CreateStringControl(wxWindow* pane, std::vector<std::string>* val, size_t index,
                                                std::function<void()> rebuildUI)
{
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    wxTextCtrl* stringCtrl = new wxTextCtrl(pane, wxID_ANY, (*val)[index]);
    elemRow->Add(stringCtrl, 1, wxEXPAND | wxRIGHT, 5);

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    stringCtrl->Bind(wxEVT_TEXT,
                     [val, index](wxCommandEvent& evt)
                     {
                         if (index < val->size())
                             (*val)[index] = evt.GetString().ToStdString();
                         evt.Skip();
                     });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               this->CallAfter(rebuildUI);
                           }
                       });

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateIntControl(wxWindow* pane, std::vector<int>* val, size_t index, std::function<void()> rebuildUI)
{
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    wxSpinCtrl* spinCtrl = new wxSpinCtrl(pane, wxID_ANY, wxString::Format("%d", (*val)[index]));
    elemRow->Add(spinCtrl, 1, wxEXPAND | wxRIGHT, 5);

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    spinCtrl->Bind(wxEVT_SPINCTRL,
                   [val, index](wxSpinEvent& evt)
                   {
                       if (index < val->size())
                           (*val)[index] = evt.GetPosition();
                   });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               this->CallAfter(rebuildUI);
                           }
                       });

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateFloatControl(wxWindow* pane, std::vector<float>* val, size_t index,
                                               std::function<void()> rebuildUI)
{
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    // Kontrolka tekstowa do edycji floatów
    //    wxTextCtrl* floatCtrl = new wxTextCtrl(pane, wxID_ANY, wxString::Format("%f", (*val)[index]));
    //    elemRow->Add(floatCtrl, 1, wxEXPAND | wxRIGHT, 5);

    wxSpinCtrlDouble* floatCtrl = new wxSpinCtrlDouble(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
    // floatCtrl->SetRange(-1000.0, 1000.0);
    floatCtrl->SetValue(1.23);
    floatCtrl->SetIncrement(0.01);
    floatCtrl->SetDigits(2);
    elemRow->Add(floatCtrl, 1, wxEXPAND | wxRIGHT, 5);

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    // Obsługa zmiany wartości float
    floatCtrl->Bind(wxEVT_TEXT,
                    [val, index](wxCommandEvent& evt)
                    {
                        if (index < val->size())
                        {
                            wxString valStr = evt.GetString();
                            double parsedVal;
                            if (valStr.ToDouble(&parsedVal))
                            {
                                (*val)[index] = static_cast<float>(parsedVal);
                            }
                            // jeśli parsowanie się nie powiedzie, ignoruj zmianę
                        }
                        evt.Skip();
                    });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               this->CallAfter(rebuildUI);
                           }
                       });

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateFileControl(wxWindow* pane, std::vector<GameEngine::File>* val, size_t index,
                                              std::function<void()> rebuildUI)
{
    auto& editedFile    = (*val)[index];
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(pane, wxID_ANY, "File:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    wxTextCtrl* fileCtrl =
        new wxTextCtrl(pane, wxID_ANY, editedFile.GetDataRelativeDir(), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    row->Add(fileCtrl, 1);
    wxButton* browseBtn = new wxButton(pane, wxID_ANY, "Browse");
    row->Add(browseBtn, 0, wxLEFT, 5);
    elemRow->Add(row, 1, wxEXPAND | wxRIGHT, 5);

    browseBtn->Bind(wxEVT_BUTTON,
                    [fileCtrl, pane](wxCommandEvent&)
                    {
                        wxFileDialog openFileDialog(pane, "Choose file", "", "", "*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                        if (openFileDialog.ShowModal() == wxID_OK)
                        {
                            fileCtrl->SetValue(openFileDialog.GetPath());
                        }
                    });

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    // Obsługa zmiany wartości float
    fileCtrl->Bind(wxEVT_TEXT_ENTER,
                   [editedFile](wxCommandEvent& evt) mutable
                   {
                       wxString valStr = evt.GetString();
                       editedFile      = GameEngine::File(valStr.ToStdString());
                       // jeśli parsowanie się nie powiedzie, ignoruj zmianę

                       evt.Skip();
                   });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               this->CallAfter(rebuildUI);
                           }
                       });

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateTextureControl(wxWindow* pane, std::vector<GameEngine::File>* val, size_t index,
                                                 std::function<void()> rebuildUI)
{
    auto& editedFile    = (*val)[index];
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

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

    elemRow->Add(row, 0, wxEXPAND | wxALL, 5);

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    // Obsługa zmiany wartości float
    textureCtrl->Bind(wxEVT_TEXT_ENTER,
                      [editedFile](wxCommandEvent& evt) mutable
                      {
                          wxString valStr = evt.GetString();
                          editedFile      = GameEngine::File(valStr.ToStdString());
                          // jeśli parsowanie się nie powiedzie, ignoruj zmianę

                          evt.Skip();
                      });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               this->CallAfter(rebuildUI);
                           }
                       });

    return elemRow;
}

void ComponentPanel::CreateUIForVectorOfStrings(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                                const GameEngine::Components::FieldInfo& field)
{
    CreateUIForVector<std::string>(component, pane, sizer, field,
                                   [this](auto p, auto v, auto i, auto r) { return this->CreateStringControl(p, v, i, r); });
}

void ComponentPanel::CreateUIForVectorOfInts(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                             const GameEngine::Components::FieldInfo& field)
{
    CreateUIForVector<int>(component, pane, sizer, field,
                           [this](auto p, auto v, auto i, auto r) { return this->CreateIntControl(p, v, i, r); });
}

void ComponentPanel::CreateUIForVectorOfFloats(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                               const GameEngine::Components::FieldInfo& field)
{
    CreateUIForVector<float>(component, pane, sizer, field,
                             [this](auto p, auto v, auto i, auto r) { return this->CreateFloatControl(p, v, i, r); });
}

void ComponentPanel::CreateUIForVectorOfTextures(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                                 const GameEngine::Components::FieldInfo& field)
{
    CreateUIForVector<GameEngine::File>(
        component, pane, sizer, field, [this](auto p, auto v, auto i, auto r) { return this->CreateTextureControl(p, v, i, r); });
}

void ComponentPanel::CreateUIForVectorOfFiles(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                              const GameEngine::Components::FieldInfo& field)
{
    CreateUIForVector<GameEngine::File>(component, pane, sizer, field,
                                        [this](auto p, auto v, auto i, auto r) { return this->CreateFileControl(p, v, i, r); });
}
