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

ComponentPanel::ComponentPanel(wxWindow* parent, GameEngine::Components::ComponentController& componentController,
                               IdType gameObjectId)
    : wxPanel(parent, wxID_ANY)
    , componentController{componentController}
    , gameObjectId{gameObjectId}
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

    // Aktualizacja layoutu po zwinięciu/rozwinieciu
    collapsible->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED,
                      [this](wxCollapsiblePaneEvent& evt)
                      {
                          this->Layout();
                          this->FitInside();
                          if (GetParent())
                              GetParent()->Layout();
                          evt.Skip();
                      });

    this->Layout();
    this->FitInside();
}

void ComponentPanel::CreateUIForComponent(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer)
{
    // Checkbox aktywności
    {
        wxCheckBox* activeCheck = new wxCheckBox(pane, wxID_ANY, "Active");
        activeCheck->SetValue(component.IsActive());
        sizer->Add(activeCheck, 0, wxALL, 5);

        activeCheck->Bind(wxEVT_CHECKBOX,
                          [this, &component](wxCommandEvent& e)
                          {
                              component.SetActive(e.IsChecked());
                              reInitComponent(component);
                          });
    }

    // Pola komponentu
    for (auto& field : component.GetFields())
        CreateUIForField(component, pane, sizer, field);
}

void ComponentPanel::CreateUIForField(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                      const GameEngine::Components::FieldInfo& field)
{
    using namespace GameEngine::Components;

    switch (field.type)
    {
        // == Skalary ==
        case FieldType::UInt:
        case FieldType::Int:
        {
            int* val = static_cast<int*>(field.ptr);

            // spin int
            wxSpinCtrl* ctrl = new wxSpinCtrl(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
            ctrl->SetRange(-1000, 1000);
            ctrl->SetValue(*val);

            wxBoxSizer* row = CreateLabeledRow(pane, field.name, ctrl);
            sizer->Add(row, 0, wxEXPAND | wxALL, 5);

            ctrl->Bind(wxEVT_SPINCTRL,
                       [this, &component, val](wxSpinEvent& evt)
                       {
                           *val = evt.GetValue();
                           reInitComponent(component);
                       });
            break;
        }

        case FieldType::Float:
        {
            float* val = static_cast<float*>(field.ptr);

            auto* ctrl = new wxSpinCtrlDouble(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
            ctrl->SetRange(-1000.0, 1000.0);
            ctrl->SetDigits(2);
            ctrl->SetIncrement(0.01);
            ctrl->SetValue(*val);

            wxBoxSizer* row = CreateLabeledRow(pane, field.name, ctrl);
            sizer->Add(row, 0, wxEXPAND | wxALL, 5);

            ctrl->Bind(wxEVT_SPINCTRLDOUBLE,
                       [this, &component, val](wxSpinDoubleEvent& evt)
                       {
                           *val = static_cast<float>(evt.GetValue());
                           reInitComponent(component);
                       });
            break;
        }

        case FieldType::String:
        {
            auto* val = static_cast<std::string*>(field.ptr);

            wxTextCtrl* ctrl = createTextEnterCtrl(pane, *val);
            wxBoxSizer* row  = CreateLabeledRow(pane, field.name, ctrl);
            sizer->Add(row, 0, wxEXPAND | wxALL, 5);

            ctrl->Bind(wxEVT_TEXT_ENTER,
                       [this, &component, val](wxCommandEvent& evt)
                       {
                           *val = evt.GetString().ToStdString();
                           reInitComponent(component);
                       });
            break;
        }

        case FieldType::Bool:
        {
            auto* val = static_cast<bool*>(field.ptr);

            wxCheckBox* check = new wxCheckBox(pane, wxID_ANY, field.name);
            check->SetValue(*val);
            sizer->Add(check, 0, wxALL, 5);

            check->Bind(wxEVT_CHECKBOX,
                        [this, val, &component](wxCommandEvent& e)
                        {
                            *val = e.IsChecked();
                            reInitComponent(component);
                        });
            break;
        }

        case FieldType::File:
        {
            auto* val = static_cast<GameEngine::File*>(field.ptr);

            auto row = CreateBrowseFileRow(pane, field.name, val->GetDataRelativeDir());
            sizer->Add(row.row, 0, wxEXPAND | wxALL, 5);

            // Browse action
            row.browseBtn->Bind(wxEVT_BUTTON, [this, &component, txt=row.textCtrl, pane, val](wxCommandEvent& evt)
                                { this->browseFileControlAction(evt, component, txt, pane, val); });

            // Enter w polu
            row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                               [this, &component, val](wxCommandEvent& evt)
                               {
                                   val->Change(evt.GetString().ToStdString());
                                   reInitComponent(component);
                               });
            break;
        }

        case FieldType::Texture:
        {
            auto* val = static_cast<GameEngine::File*>(field.ptr);

            auto row = CreateBrowseTextureRow(pane, field.name, val->GetDataRelativeDir());
            sizer->Add(row.row, 0, wxEXPAND | wxALL, 5);

            // Browse action
            row.browseBtn->Bind(wxEVT_BUTTON,
                                [this, &component, tr=row.textCtrl, prev=row.preview, pane, val](wxCommandEvent&)
                                {
                                    wxFileDialog openFileDialog(pane, "Choose texture", "", "",
                                                                "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                                    if (openFileDialog.ShowModal() == wxID_OK)
                                    {
                                        wxString path = openFileDialog.GetPath();
                                        tr->SetValue(path);
                                        val->Change(path.ToStdString());
                                        reInitComponent(component);
                                        SetPreviewBitmap(prev, path, pane);
                                    }
                                });

            // Enter w polu
            row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                               [this, &component, val, prev=row.preview, pane](wxCommandEvent& evt)
                               {
                                   val->Change(evt.GetString().ToStdString());
                                   reInitComponent(component);
                                   SetPreviewBitmap(prev, evt.GetString(), pane);
                               });
            break;
        }

        // == Wektory ==
        case FieldType::VectorOfStrings:
            CreateUIForVector<std::string>(component, pane, sizer, field,
                                           [this, &component](auto p, auto v, auto i, auto r)
                                           { return this->CreateStringItem(component, p, v, i, r); });
            break;

        case FieldType::VectorOfInt:
            CreateUIForVector<int>(component, pane, sizer, field,
                                   [this, &component](auto p, auto v, auto i, auto r)
                                   { return this->CreateIntItem(component, p, v, i, r); });
            break;

        case FieldType::VectorOfFloat:
            CreateUIForVector<float>(component, pane, sizer, field,
                                     [this, &component](auto p, auto v, auto i, auto r)
                                     { return this->CreateFloatItem(component, p, v, i, r); });
            break;

        case FieldType::VectorOfFiles:
            CreateUIForVector<GameEngine::File>(component, pane, sizer, field,
                                                [this, &component](auto p, auto v, auto i, auto r)
                                                { return this->CreateFileItem(component, p, v, i, r); });
            break;

        case FieldType::VectorOfTextures:
            CreateUIForVector<GameEngine::File>(component, pane, sizer, field,
                                                [this, &component](auto p, auto v, auto i, auto r)
                                                { return this->CreateTextureItem(component, p, v, i, r); });
            break;
    }
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

    // Pasek rozmiaru
    wxBoxSizer* sizeRow     = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* sizeLabel = new wxStaticText(pane, wxID_ANY, "Vector size:");
    wxTextCtrl* sizeCtrl = new wxTextCtrl(pane, wxID_ANY, wxString::Format("%zu", val->size()),
                                          wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    sizeRow->Add(sizeLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    sizeRow->Add(sizeCtrl, 0, wxALIGN_CENTER_VERTICAL);
    valuesSizer->Add(sizeRow, 0, wxBOTTOM, 10);

    auto rebuildUI = [this, &component]()
    {
        mainSizer->Clear(true);
        AddComponent(component);
        reInitComponent(component);

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
                           if (requestedSize < 0) requestedSize = 0;
                           if (requestedSize != static_cast<int>(val->size()))
                           {
                               val->resize(static_cast<size_t>(requestedSize));
                               this->CallAfter(rebuildUI);
                           }
                       }
                       catch (...) { /* ignoruj błędy parsowania */ }
                   });

    // Elementy
    for (size_t i = 0; i < val->size(); ++i)
    {
        wxBoxSizer* elemRow = createElementControl(pane, val, i, rebuildUI);
        valuesSizer->Add(elemRow, 0, wxEXPAND | wxBOTTOM, 3);
    }

    // Dodawanie elementu
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

wxBoxSizer* ComponentPanel::CreateStringItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                             std::vector<std::string>* val, size_t index, std::function<void()> rebuildUI)
{
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    wxTextCtrl* stringCtrl = createTextEnterCtrl(pane, (*val)[index]);
    elemRow->Add(stringCtrl, 1, wxEXPAND | wxRIGHT, 5);

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    stringCtrl->Bind(wxEVT_TEXT_ENTER,
                     [this, &component, val, index](wxCommandEvent& evt)
                     {
                         if (index < val->size())
                         {
                             (*val)[index] = evt.GetString().ToStdString();
                             reInitComponent(component);
                         }
                         evt.Skip();
                     });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, &component, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               this->CallAfter(rebuildUI);
                               reInitComponent(component);
                           }
                       });

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateIntItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                          std::vector<int>* val, size_t index, std::function<void()> rebuildUI)
{
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    wxSpinCtrl* spinCtrl = new wxSpinCtrl(pane, wxID_ANY, wxString::Format("%d", (*val)[index]));
    elemRow->Add(spinCtrl, 1, wxEXPAND | wxRIGHT, 5);

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    spinCtrl->Bind(wxEVT_SPINCTRL,
                   [this, &component, val, index](wxSpinEvent& evt)
                   {
                       if (index < val->size())
                       {
                           (*val)[index] = evt.GetPosition();
                           reInitComponent(component);
                       }
                   });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, &component, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               reInitComponent(component);
                               this->CallAfter(rebuildUI);
                           }
                       });

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateFloatItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                            std::vector<float>* val, size_t index, std::function<void()> rebuildUI)
{
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    wxSpinCtrlDouble* floatCtrl = new wxSpinCtrlDouble(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
    floatCtrl->SetIncrement(0.01);
    floatCtrl->SetDigits(2);
    floatCtrl->SetValue((*val)[index]);
    elemRow->Add(floatCtrl, 1, wxEXPAND | wxRIGHT, 5);

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    floatCtrl->Bind(wxEVT_SPINCTRLDOUBLE,
                    [this, &component, val, index](wxSpinDoubleEvent& evt)
                    {
                        if (index < val->size())
                        {
                            (*val)[index] = static_cast<float>(evt.GetValue());
                            reInitComponent(component);
                        }
                    });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, &component, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               reInitComponent(component);
                               this->CallAfter(rebuildUI);
                           }
                       });

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateFileItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                           std::vector<GameEngine::File>* val, size_t index, std::function<void()> rebuildUI)
{
    auto& editedFile    = (*val)[index];
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    auto row = CreateBrowseFileRow(pane, "File:", editedFile.GetDataRelativeDir());
    elemRow->Add(row.row, 1, wxEXPAND | wxRIGHT, 5);

    row.browseBtn->Bind(wxEVT_BUTTON,
                        [this, &component, txt=row.textCtrl, pane, &editedFile](wxCommandEvent& evt)
                        { this->browseFileControlAction(evt, component, txt, pane, &editedFile); });

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                       [this, &component, editedFile](wxCommandEvent& evt) mutable
                       {
                           editedFile = GameEngine::File(evt.GetString().ToStdString());
                           reInitComponent(component);
                           evt.Skip();
                       });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, &component, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               reInitComponent(component);
                               this->CallAfter(rebuildUI);
                           }
                       });

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateTextureItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                              std::vector<GameEngine::File>* val, size_t index,
                                              std::function<void()> rebuildUI)
{
    auto& editedFile    = (*val)[index];
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    auto row = CreateBrowseTextureRow(pane, "Texture:", editedFile.GetDataRelativeDir());
    elemRow->Add(row.row, 1, wxEXPAND | wxRIGHT, 5);

    row.browseBtn->Bind(wxEVT_BUTTON,
                        [this, &component, tr=row.textCtrl, prev=row.preview, pane, &editedFile](wxCommandEvent&)
                        {
                            wxFileDialog openFileDialog(pane, "Choose texture", "", "",
                                                        "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                            if (openFileDialog.ShowModal() == wxID_OK)
                            {
                                wxString path = openFileDialog.GetPath();
                                tr->SetValue(path);
                                editedFile = GameEngine::File(path.ToStdString());
                                reInitComponent(component);
                                SetPreviewBitmap(prev, path, pane);
                            }
                        });

    wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
    removeButton->SetToolTip("Remove element");
    elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

    row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                       [this, &component, &editedFile, prev=row.preview, pane](wxCommandEvent& evt) mutable
                       {
                           editedFile = GameEngine::File(evt.GetString().ToStdString());
                           reInitComponent(component);
                           SetPreviewBitmap(prev, evt.GetString(), pane);
                           evt.Skip();
                       });

    removeButton->Bind(wxEVT_BUTTON,
                       [this, &component, val, index, rebuildUI](wxCommandEvent&)
                       {
                           if (index < val->size())
                           {
                               val->erase(val->begin() + index);
                               reInitComponent(component);
                               this->CallAfter(rebuildUI);
                           }
                       });

    return elemRow;
}

void ComponentPanel::reInitComponent(GameEngine::Components::IComponent& component)
{
    component.CleanUp();
    this->componentController.CallGameObjectFunctions(GameEngine::Components::FunctionType::Awake, gameObjectId);
}

void ComponentPanel::browseFileControlAction(wxCommandEvent&, GameEngine::Components::IComponent& component,
                                             wxTextCtrl* fileCtrl, wxWindow* pane, GameEngine::File* val)
{
    wxFileDialog openFileDialog(pane, "Choose file", "", "", "*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_OK)
    {
        fileCtrl->SetValue(openFileDialog.GetPath());
        val->Change(openFileDialog.GetPath().ToStdString());
        reInitComponent(component);
    }
}

wxBoxSizer* ComponentPanel::CreateLabeledRow(wxWindow* parent, const wxString& label, wxWindow* control,
                                             int proportion, int flags, int border)
{
    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(parent, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    row->Add(control, proportion, flags, border);
    return row;
}

wxTextCtrl *ComponentPanel::createTextEnterCtrl(wxWindow *pane, const std::string &text)
{
    return new wxTextCtrl(pane, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
}

ComponentPanel::BrowseRow ComponentPanel::CreateBrowseFileRow(wxWindow* parent, const wxString& label,
                                                              const wxString& initial)
{
    BrowseRow out{};
    out.row = new wxBoxSizer(wxHORIZONTAL);
    out.row->Add(new wxStaticText(parent, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    out.textCtrl = createTextEnterCtrl(parent, initial.ToStdString());
    out.row->Add(out.textCtrl, 1);

    out.browseBtn = new wxButton(parent, wxID_ANY, "Browse");
    out.row->Add(out.browseBtn, 0, wxLEFT, 5);

    return out;
}

ComponentPanel::TextureRow ComponentPanel::CreateBrowseTextureRow(wxWindow* parent, const wxString& label,
                                                                  const wxString& initial)
{
    TextureRow out{};
    out.row = new wxBoxSizer(wxHORIZONTAL);

    out.row->Add(new wxStaticText(parent, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    out.textCtrl = createTextEnterCtrl(parent, initial.ToStdString());
    out.row->Add(out.textCtrl, 1, wxALIGN_CENTER_VERTICAL);

    out.browseBtn = new wxButton(parent, wxID_ANY, "Browse");
    out.row->Add(out.browseBtn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    out.preview = new wxStaticBitmap(parent, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(64, 64));
    out.row->Add(out.preview, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 10);

    return out;
}

void ComponentPanel::SetPreviewBitmap(wxStaticBitmap* preview, const wxString& path, wxWindow* relayoutParent)
{
    wxImage img;
    if (img.LoadFile(path))
    {
        wxBitmap bmp(img.Scale(64, 64));
        preview->SetBitmap(bmp);
        if (relayoutParent) relayoutParent->Layout();
    }
    else
    {
        wxMessageBox("Failed to load image.", "Error", wxICON_ERROR);
    }
}
