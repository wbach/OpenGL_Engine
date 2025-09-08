#include "ComponentPanel.h"

#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ExternalComponentsReader.h>
#include <GameEngine/Objects/GameObject.h>
#include <Logger/Log.h>
#include <Utils/TreeNode.h>
#include <wx/artprov.h>
#include <wx/dnd.h>
#include <wx/wx.h>

#include <magic_enum/magic_enum.hpp>

#include "MyEvents.h"
#include "Resources/File.h"
#include "ThumbnailCache.h"

class MyTextDropTarget : public wxTextDropTarget
{
public:
    MyTextDropTarget(wxTextCtrl* ctrl)
        : ctrl(ctrl)
    {
    }

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data) override
    {
        LOG_DEBUG << "Drop text" << data;
        if (ctrl)
        {
            ctrl->SetValue(data);
            return true;
        }
        return false;
    }

private:
    wxTextCtrl* ctrl;
};

class MyFileDropTarget : public wxFileDropTarget
{
public:
    using Callback = std::function<void(const std::string&)>;
    MyFileDropTarget(wxTextCtrl* textCtrl, Callback callback)
        : ctrl(textCtrl)
        , callback{callback}
    {
    }

    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override
    {
        LOG_DEBUG << "Drop text" << filenames[0];
        if (ctrl && !filenames.IsEmpty())
        {
            ctrl->ChangeValue(GameEngine::File(filenames[0]).GetDataRelativeDir());
            ctrl->SetToolTip(filenames[0]);

            if (callback)
                callback(filenames[0].ToStdString());
        }
        return true;
    }

private:
    wxTextCtrl* ctrl;
    Callback callback;
};

namespace GameEngine
{
namespace Components
{
extern const std::string CSTR_TYPE;
}
}  // namespace GameEngine

ComponentPanel::ComponentPanel(wxFrame* mainFrame, wxWindow* parent, GameEngine::ExternalComponentsReader& reader,
                               GameEngine::Components::ComponentController& componentController,
                               GameEngine::GameObject& gameObject)
    : wxPanel(parent, wxID_ANY)
    , mainFrame(mainFrame)
    , externalComponentsReader(reader)
    , componentController{componentController}
    , gameObject{gameObject}
{
    mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);
}

void ComponentPanel::Lock()
{
    std::function<void(wxWindow*)> lockChildren = [&](wxWindow* parent)
    {
        if (not parent)
            return;

        if (not(parent == headerPanel || parent == collapsible))
            parent->Disable();

        for (auto child : parent->GetChildren())
            lockChildren(child);
    };

    for (auto child : GetChildren())
        lockChildren(child);
}

void ComponentPanel::Unlock()
{
    std::function<void(wxWindow*)> enableChildren;
    enableChildren = [&](wxWindow* parent)
    {
        parent->Enable();
        wxWindowList& children = parent->GetChildren();
        for (auto child : children)
        {
            enableChildren(child);
        }
    };
    enableChildren(this);
}

void ComponentPanel::ClearComponents()
{
    mainSizer->Clear(true);
    Layout();
}

void ComponentPanel::AddComponent(GameEngine::Components::IComponent& component, bool collapsed)
{
    TreeNode node("component");
    component.write(node);
    auto typeName = node.getAttributeValue(GameEngine::Components::CSTR_TYPE);

    headerPanel             = new wxPanel(this);
    wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

    // Label z nazwą sekcji
    wxStaticText* title = new wxStaticText(headerPanel, wxID_ANY, "Component name: " + typeName);
    headerSizer->Add(title, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);

    // Checkbox aktywności
    {
        auto* activeCheck = new wxCheckBox(headerPanel, wxID_ANY, "Active");
        activeCheck->SetValue(component.IsActive());
        headerSizer->Add(activeCheck, 0, wxALL, 5);

        activeCheck->Bind(wxEVT_CHECKBOX,
                          [this, &component](auto& e)
                          {
                              component.SetActive(e.IsChecked());
                              reInitComponent(component);
                          });
    }

    auto externalLoadedLibs = externalComponentsReader.GetLoadedLibs();
    for (auto& [file, name] : externalLoadedLibs)
    {
        if (name == component.GetTypeString())
        {
            wxButton* reloadComponentButton = new wxButton(headerPanel, wxID_ANY, "Reload", wxDefaultPosition, wxSize(60, 20));
            headerSizer->Add(reloadComponentButton, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
            reloadComponentButton->Bind(wxEVT_BUTTON,
                                        [this, libFile = file, typeName](const auto&)
                                        {
                                            int answer = wxMessageBox("Reload component " + typeName + "?", "Confirmation",
                                                                      wxYES_NO | wxICON_QUESTION);
                                            if (answer == wxYES)
                                            {
                                                CallAfter(
                                                    [this, libFile]()
                                                    {
                                                        ReloadComponentLibEvent event(wxEVT_RELOAD_COMPONENT_LIB_EVENT);
                                                        event.SetFile(libFile);
                                                        event.SetGameObject(gameObject);
                                                        wxPostEvent(mainFrame, event);
                                                    });
                                            }
                                        });
            break;
        }
    }

    wxButton* deleteComponentButton = new wxButton(headerPanel, wxID_ANY, "Delete", wxDefaultPosition, wxSize(60, 20));
    headerSizer->Add(deleteComponentButton, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    deleteComponentButton->Bind(wxEVT_BUTTON,
                                [this, type = component.GetType(), typeName](const auto&)
                                {
                                    int answer = wxMessageBox("Delete component " + typeName + "?", "Confirmation",
                                                              wxYES_NO | wxICON_QUESTION);
                                    if (answer == wxYES)
                                    {
                                        CallAfter(
                                            [this, type]()
                                            {
                                                gameObject.RemoveComponent(type);
                                                mainSizer->Clear(true);
                                                this->Layout();
                                                this->FitInside();
                                                if (this->GetParent())
                                                {
                                                    this->GetParent()->Layout();
                                                    this->GetParent()->FitInside();
                                                }
                                            });
                                    }
                                });

    headerPanel->SetSizer(headerSizer);

    // Tworzymy collapsible pane
    collapsible    = new wxCollapsiblePane(this, wxID_ANY, "");
    wxWindow* pane = collapsible->GetPane();

    // Sizer dla zawartości collapsible
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    pane->SetSizer(sizer);

    CreateUIForComponent(component, pane, sizer);

    pane->Layout();
    sizer->Layout();

    // Dodajemy do głównego sizer
    mainSizer->Add(headerPanel, 0, wxEXPAND | wxALL, 0);
    mainSizer->Add(collapsible, 0, wxEXPAND | wxALL, 0);
    collapsible->Collapse(false);

    auto onCollapsAction = [this](auto& evt)
    {
        this->Layout();
        this->FitInside();
        if (GetParent())
        {
            GetParent()->Layout();
            GetParent()->FitInside();
            if (GetParent()->GetParent())
            {
                GetParent()->GetParent()->Layout();
                GetParent()->GetParent()->GetParent();
            }
        }

        evt.Skip();
    };
    // Aktualizacja layoutu po zwinięciu/rozwinieciu
    collapsible->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, onCollapsAction);
    if (collapsed)
        collapsible->Collapse(true);

    this->Layout();
    this->FitInside();
}

void ComponentPanel::CreateUIForComponent(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer)
{
    if (not component.GetFields().empty())
    {
        for (auto& field : component.GetFields())
            CreateUIForField(component, pane, sizer, field);
    }
    else
    {
        wxStaticText* title = new wxStaticText(pane, wxID_ANY, "No fields in component");
        sizer->Add(title, 0, wxEXPAND | wxALL, 0);
    }
}

void ComponentPanel::CreateUIForField(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                      const GameEngine::Components::FieldInfo& field)
{
    using namespace GameEngine::Components;

    LOG_DEBUG << "Field type : " << magic_enum::enum_name(field.type);
    switch (field.type)
    {
        case FieldType::Enum:
        {
            auto* ctrl = new wxChoice(pane, wxID_ANY);

            for (auto& n : field.enumNames())
                ctrl->Append(wxString::FromUTF8(n.c_str()));
            ctrl->SetSelection(field.enumToIndex(field.ptr));

            auto* row = CreateLabeledRow(pane, field.name, ctrl);
            sizer->Add(row, 0, wxEXPAND | wxALL, 5);

            ctrl->Bind(wxEVT_CHOICE,
                       [this, &component, ptr = field.ptr, indexToEnum = field.indexToEnum](wxCommandEvent& evt)
                       {
                           LOG_DEBUG << "OnChoice";
                           indexToEnum(ptr, evt.GetSelection());
                           LOG_DEBUG << "reInit";
                           reInitComponent(component);
                       });

            break;
        }
        case GameEngine::Components::FieldType::AnimationClip:
        {
            auto* val      = static_cast<GameEngine::Components::ReadAnimationInfo*>(field.ptr);
            auto clipSizer = CreateUIForAnimationClip(component, pane, val);
            sizer->Add(clipSizer, 0, wxEXPAND | wxALL, 5);
            break;
        }
        break;
        case GameEngine::Components::FieldType::VectorOfAnimationClips:
            CreateUIForVector<GameEngine::Components::ReadAnimationInfo>(
                component, pane, sizer, field,
                [this, &component](auto p, auto v, auto i, auto r, auto del)
                { return this->CreateAnimationClipItem(component, p, v, i, r, del); });
            break;
        case FieldType::UInt:
        case FieldType::Int:
        {
            auto* val = static_cast<int*>(field.ptr);

            auto* ctrl = new wxSpinCtrl(pane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
            ctrl->Bind(wxEVT_MOUSEWHEEL, [](auto& evt) {});
            ctrl->SetRange(-1000, 1000);
            ctrl->SetValue(*val);

            auto* row = CreateLabeledRow(pane, field.name, ctrl);
            sizer->Add(row, 0, wxEXPAND | wxALL, 5);

            ctrl->Bind(wxEVT_SPINCTRL,
                       [this, &component, val](auto& evt)
                       {
                           *val = evt.GetValue();
                           reInitComponent(component);
                       });
            break;
        }

        case FieldType::Float:
        {
            auto val  = static_cast<float*>(field.ptr);
            auto ctrl = CreateFloatSpinCtrl(pane, *val, -1000.0, 1000.0, 0.01, 2);
            auto row  = CreateLabeledRow(pane, field.name, ctrl);
            sizer->Add(row, 0, wxEXPAND | wxALL, 5);

            ctrl->Bind(wxEVT_SPINCTRLDOUBLE,
                       [this, &component, val](auto& evt)
                       {
                           *val = static_cast<float>(evt.GetValue());
                           reInitComponent(component);
                       });
            break;
        }

        case FieldType::String:
        {
            auto* val  = static_cast<std::string*>(field.ptr);
            auto* ctrl = createTextEnterCtrl(pane, *val);
            auto* row  = CreateLabeledRow(pane, field.name, ctrl);
            sizer->Add(row, 0, wxEXPAND | wxALL, 5);

            ctrl->Bind(wxEVT_TEXT_ENTER,
                       [this, &component, val](auto& evt)
                       {
                           *val = evt.GetString().ToStdString();
                           reInitComponent(component);
                       });
            break;
        }

        case FieldType::Boolean:
        {
            auto* val   = static_cast<bool*>(field.ptr);
            auto* check = new wxCheckBox(pane, wxID_ANY, field.name);
            check->SetValue(*val);
            sizer->Add(check, 0, wxALL, 5);

            check->Bind(wxEVT_CHECKBOX,
                        [this, val, &component](auto& e)
                        {
                            *val = e.IsChecked();
                            reInitComponent(component);
                        });
            break;
        }

        case FieldType::File:
        {
            auto* val = static_cast<GameEngine::File*>(field.ptr);
            auto row  = CreateBrowseFileRow(pane, field.name, val->GetDataRelativeDir());
            sizer->Add(row.row, 0, wxEXPAND | wxALL, 5);
            row.textCtrl->SetToolTip(row.textCtrl->GetValue());
            // Browse action
            row.browseBtn->Bind(wxEVT_BUTTON, [this, &component, txt = row.textCtrl, pane, val](auto& evt)
                                { this->browseFileControlAction(evt, component, txt, pane, val); });

            // Enter w polu
            row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                               [this, &component, val, txt = row.textCtrl](auto& evt)
                               {
                                   val->Change(evt.GetString().ToStdString());
                                   reInitComponent(component);
                                   txt->SetToolTip(txt->GetValue());
                               });

            row.textCtrl->SetDropTarget(new MyFileDropTarget(row.textCtrl,
                                                             [this, &component, val](const std::string& path)
                                                             {
                                                                 val->Change(path);
                                                                 reInitComponent(component);
                                                             }));
            break;
        }

        case FieldType::Texture:
        {
            auto* val = static_cast<GameEngine::File*>(field.ptr);

            auto row = CreateBrowseTextureRow(pane, field.name, val->GetDataRelativeDir());
            sizer->Add(row.row, 0, wxEXPAND | wxALL, 5);

            SetPreviewBitmap(row.preview, *val, pane);
            row.textCtrl->SetToolTip(row.textCtrl->GetValue());
            // Browse action
            row.browseBtn->Bind(wxEVT_BUTTON,
                                [this, &component, txt = row.textCtrl, prev = row.preview, pane, val](wxCommandEvent&)
                                {
                                    wxFileDialog openFileDialog(pane, "Choose texture", EngineConf.files.data, "",
                                                                "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                                    if (openFileDialog.ShowModal() == wxID_OK)
                                    {
                                        wxString path = openFileDialog.GetPath();
                                        val->Change(path.ToStdString());
                                        txt->SetValue(val->GetDataRelativeDir());
                                        reInitComponent(component);
                                        SetPreviewBitmap(prev, GameEngine::File{path.ToStdString()}, pane);
                                        txt->SetToolTip(txt->GetValue());
                                    }
                                });

            // Enter w polu
            row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                               [this, &component, val, prev = row.preview, pane, txt = row.textCtrl](wxCommandEvent& evt)
                               {
                                   val->Change(evt.GetString().ToStdString());
                                   reInitComponent(component);
                                   SetPreviewBitmap(prev, GameEngine::File{evt.GetString().ToStdString()}, pane);
                                   txt->SetToolTip(txt->GetValue());
                                   txt->SetValue(val->GetDataRelativeDir());
                               });

            row.textCtrl->SetDropTarget(new MyFileDropTarget(row.textCtrl,
                                                             [this, &component, val](const std::string& path)
                                                             {
                                                                 val->Change(path);
                                                                 reInitComponent(component);
                                                             }));
            break;
        }

        case GameEngine::Components::FieldType::Vector2i:
        {
            auto* val = static_cast<vec2i*>(field.ptr);
            CreateVectorRow<vec2i, wxSpinCtrl>(
                component, pane, sizer, field.name, val, 2,
                [](wxSpinCtrl* ctrl, int v)
                {
                    ctrl->SetRange(-100000, 100000);
                    ctrl->SetValue(v);
                },
                [](wxSpinEvent& e) { return e.GetValue(); },
                [](wxSpinCtrl* ctrl, auto handler) { ctrl->Bind(wxEVT_SPINCTRL, handler); });
            break;
        }

        case GameEngine::Components::FieldType::Vector2f:
        {
            auto* val = static_cast<vec2*>(field.ptr);
            CreateVectorRow<vec2, wxSpinCtrlDouble>(
                component, pane, sizer, field.name, val, 2,
                [](wxSpinCtrlDouble* ctrl, float v)
                {
                    ctrl->SetRange(-100000.0, 100000.0);
                    ctrl->SetDigits(3);
                    ctrl->SetIncrement(0.01);
                    ctrl->SetValue(v);
                },
                [](wxSpinDoubleEvent& e) { return static_cast<float>(e.GetValue()); },
                [](wxSpinCtrlDouble* ctrl, auto handler) { ctrl->Bind(wxEVT_SPINCTRLDOUBLE, handler); });
            break;
        }
        case GameEngine::Components::FieldType::Vector3f:
        {
            auto* val = static_cast<vec3*>(field.ptr);
            CreateVectorRow<vec3, wxSpinCtrlDouble>(
                component, pane, sizer, field.name, val, 3,
                [](wxSpinCtrlDouble* ctrl, float v)
                {
                    ctrl->SetRange(-100000.0, 100000.0);
                    ctrl->SetDigits(3);
                    ctrl->SetIncrement(0.01);
                    ctrl->SetValue(v);
                },
                [](wxSpinDoubleEvent& e) { return static_cast<float>(e.GetValue()); },
                [](wxSpinCtrlDouble* ctrl, auto handler) { ctrl->Bind(wxEVT_SPINCTRLDOUBLE, handler); });
            break;
        }
        case GameEngine::Components::FieldType::Vector4f:
        {
            auto* val = static_cast<vec4*>(field.ptr);
            CreateVectorRow<vec4, wxSpinCtrlDouble>(
                component, pane, sizer, field.name, val, 4,
                [](wxSpinCtrlDouble* ctrl, float v)
                {
                    ctrl->SetRange(-100000.0, 100000.0);
                    ctrl->SetDigits(3);
                    ctrl->SetIncrement(0.01);
                    ctrl->SetValue(v);
                },
                [](wxSpinDoubleEvent& e) { return static_cast<float>(e.GetValue()); },
                [](wxSpinCtrlDouble* ctrl, auto handler) { ctrl->Bind(wxEVT_SPINCTRLDOUBLE, handler); });
            break;
        }
        case GameEngine::Components::FieldType::ColorRGB:
        {
            auto* val = static_cast<vec3*>(field.ptr);
            CreateVectorRow<vec3, wxSpinCtrlDouble>(
                component, pane, sizer, field.name, val, 3,
                [](wxSpinCtrlDouble* ctrl, float v)
                {
                    ctrl->SetRange(-100000.0, 100000.0);
                    ctrl->SetDigits(3);
                    ctrl->SetIncrement(0.01);
                    ctrl->SetValue(v);
                },
                [](wxSpinDoubleEvent& e) { return static_cast<float>(e.GetValue()); },
                [](wxSpinCtrlDouble* ctrl, auto handler) { ctrl->Bind(wxEVT_SPINCTRLDOUBLE, handler); }, {"R:", "G:", "B:"});
            break;
        }
        case GameEngine::Components::FieldType::ColorRGBA:
        {
            auto* val = static_cast<vec4*>(field.ptr);
            CreateVectorRow<vec4, wxSpinCtrlDouble>(
                component, pane, sizer, field.name, val, 4,
                [](wxSpinCtrlDouble* ctrl, float v)
                {
                    ctrl->SetRange(-100000.0, 100000.0);
                    ctrl->SetDigits(3);
                    ctrl->SetIncrement(0.01);
                    ctrl->SetValue(v);
                },
                [](wxSpinDoubleEvent& e) { return static_cast<float>(e.GetValue()); },
                [](wxSpinCtrlDouble* ctrl, auto handler) { ctrl->Bind(wxEVT_SPINCTRLDOUBLE, handler); },
                {"R:", "G:", "B:", "A:"});
            break;
        }
        // == Wektory ==
        case FieldType::VectorOfStrings:
            CreateUIForVector<std::string>(component, pane, sizer, field,
                                           [this, &component](auto p, auto v, auto i, auto r, auto del)
                                           { return this->CreateStringItem(component, p, v, i, r, del); });
            break;

        case FieldType::VectorOfInt:
            CreateUIForVector<int>(component, pane, sizer, field,
                                   [this, &component](auto p, auto v, auto i, auto r, auto del)
                                   { return this->CreateIntItem(component, p, v, i, r, del); });
            break;

        case FieldType::VectorOfFloat:
            CreateUIForVector<float>(component, pane, sizer, field,
                                     [this, &component](auto p, auto v, auto i, auto r, auto del)
                                     { return this->CreateFloatItem(component, p, v, i, r, del); });
            break;

        case FieldType::VectorOfFiles:
            CreateUIForVector<GameEngine::File>(component, pane, sizer, field,
                                                [this, &component](auto p, auto v, auto i, auto r, auto del)
                                                { return this->CreateFileItem(component, p, v, i, r, del); });
            break;

        case FieldType::VectorOfTextures:
            CreateUIForVector<GameEngine::File>(component, pane, sizer, field,
                                                [this, &component](auto p, auto v, auto i, auto r, auto del)
                                                { return this->CreateTextureItem(component, p, v, i, r, del); });
            break;

        case FieldType::ConstVectorOfTextures:
            CreateUIForVector<GameEngine::File>(
                component, pane, sizer, field,
                [this, &component](auto p, auto v, auto i, auto r, auto del)
                { return this->CreateTextureItem(component, p, v, i, r, del); },
                false);
            break;
    }
}

template <typename VecT, typename CtrlT, typename BindEvt, typename SetVal, typename GetVal>
void ComponentPanel::CreateVectorRow(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                     const wxString& label, VecT* vec, int dimensions, SetVal setCtrlValue, GetVal getCtrlValue,
                                     BindEvt bindEvent, const std::vector<std::string>& axisNames)
{
    // Etykieta nad polami
    sizer->Add(new wxStaticText(pane, wxID_ANY, label), 0, wxBOTTOM, 2);

    // Własny panel, żeby odseparować layout od reszty
    wxPanel* rowPanel    = new wxPanel(pane);
    wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);

    for (int i = 0; i < dimensions; ++i)
    {
        // Oś
        rowSizer->Add(new wxStaticText(rowPanel, wxID_ANY, axisNames[i]), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        // Pole - własny startowy rozmiar
        CtrlT* ctrl = new CtrlT(rowPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS);
        setCtrlValue(ctrl, (*vec)[i]);

        // Rozciąganie tylko w obrębie tego wiersza
        rowSizer->Add(ctrl, 1, wxRIGHT, (i == dimensions - 1 ? 0 : 10));

        bindEvent(ctrl,
                  [this, &component, vec, i, getCtrlValue](auto& evt)
                  {
                      (*vec)[i] = getCtrlValue(evt);
                      reInitComponent(component);
                  });
    }

    rowPanel->SetSizer(rowSizer);
    sizer->Add(rowPanel, 0, wxEXPAND | wxBOTTOM, 5);
}

template <typename T>
void ComponentPanel::CreateUIForVector(
    GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
    const GameEngine::Components::FieldInfo& field,
    std::function<wxBoxSizer*(wxWindow*, std::vector<T>*, size_t, std::function<void()>, bool)> createElementControl,
    bool resizeable)
{
    auto val = static_cast<std::vector<T>*>(field.ptr);

    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* nameLabel = new wxStaticText(pane, wxID_ANY, field.name);
    row->Add(nameLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);

    wxBoxSizer* valuesSizer = new wxBoxSizer(wxVERTICAL);

    // Pasek rozmiaru
    wxBoxSizer* sizeRow     = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* sizeLabel = new wxStaticText(pane, wxID_ANY, "Vector size:");

    long style = wxTE_PROCESS_ENTER;
    if (not resizeable)
    {
        style = wxTE_READONLY;
    }
    wxTextCtrl* sizeCtrl =
        new wxTextCtrl(pane, wxID_ANY, wxString::Format("%zu", val->size()), wxDefaultPosition, wxDefaultSize, style);

    sizeRow->Add(sizeLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    sizeRow->Add(sizeCtrl, 0, wxALIGN_CENTER_VERTICAL);
    valuesSizer->Add(sizeRow, 0, wxBOTTOM, 10);

    if (not resizeable)
        sizeCtrl->Disable();

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

    if (resizeable)
    {
        sizeCtrl->Bind(wxEVT_TEXT_ENTER,
                       [this, val, sizeCtrl, rebuildUI](auto&)
                       {
                           try
                           {
                               int requestedSize = std::stoi(sizeCtrl->GetValue().ToStdString());
                               if (requestedSize < 0)
                                   requestedSize = 0;
                               if (requestedSize != static_cast<int>(val->size()))
                               {
                                   val->resize(static_cast<size_t>(requestedSize));
                                   this->CallAfter(rebuildUI);
                               }
                           }
                           catch (...)
                           { /* ignoruj błędy parsowania */
                           }
                       });
    }

    // Elementy
    for (size_t i = 0; i < val->size(); ++i)
    {
        wxBoxSizer* elemRow = createElementControl(pane, val, i, rebuildUI, resizeable);
        valuesSizer->Add(elemRow, 0, wxEXPAND | wxBOTTOM, 3);
    }

    if (resizeable)
    {
        // Dodawanie elementu
        wxButton* addButton = new wxButton(pane, wxID_ANY, "Add element");
        valuesSizer->Add(addButton, 0, wxLEFT, 10);

        addButton->Bind(wxEVT_BUTTON,
                        [this, val, rebuildUI](const auto&)
                        {
                            val->push_back({});
                            this->CallAfter(rebuildUI);
                        });
    }

    row->Add(valuesSizer, 1, wxEXPAND);
    sizer->Add(row, 0, wxEXPAND | wxALL, 5);
}

wxBoxSizer* ComponentPanel::CreateStringItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                             std::vector<std::string>* val, size_t index, std::function<void()> rebuildUI,
                                             bool canDelete)
{
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    wxTextCtrl* stringCtrl = createTextEnterCtrl(pane, (*val)[index]);
    elemRow->Add(stringCtrl, 1, wxEXPAND | wxRIGHT, 5);

    stringCtrl->SetToolTip(stringCtrl->GetValue());
    stringCtrl->Bind(wxEVT_TEXT_ENTER,
                     [this, &component, val, index, stringCtrl](wxCommandEvent& evt)
                     {
                         if (index < val->size())
                         {
                             (*val)[index] = evt.GetString().ToStdString();
                             stringCtrl->SetToolTip(evt.GetString().ToStdString());
                             reInitComponent(component);
                         }
                         evt.Skip();
                     });

    if (canDelete)
    {
        wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
        removeButton->SetToolTip("Remove element");
        elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

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
    }

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateIntItem(GameEngine::Components::IComponent& component, wxWindow* pane, std::vector<int>* val,
                                          size_t index, std::function<void()> rebuildUI, bool canDelete)
{
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    wxSpinCtrl* spinCtrl = new wxSpinCtrl(pane, wxID_ANY, wxString::Format("%d", (*val)[index]));
    spinCtrl->Bind(wxEVT_MOUSEWHEEL, [](auto& evt) {});
    elemRow->Add(spinCtrl, 1, wxEXPAND | wxRIGHT, 5);

    spinCtrl->Bind(wxEVT_SPINCTRL,
                   [this, &component, val, index](wxSpinEvent& evt)
                   {
                       if (index < val->size())
                       {
                           (*val)[index] = evt.GetPosition();
                           reInitComponent(component);
                       }
                   });

    if (canDelete)
    {
        wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
        removeButton->SetToolTip("Remove element");
        elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

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
    }

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateFloatItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                            std::vector<float>* val, size_t index, std::function<void()> rebuildUI,
                                            bool canDelete)
{
    auto elemRow   = new wxBoxSizer(wxHORIZONTAL);
    auto floatCtrl = CreateFloatSpinCtrl(pane, (*val)[index], 0.01, 1000.0, 0.1, 2);
    elemRow->Add(floatCtrl, 1, wxEXPAND | wxRIGHT, 5);

    floatCtrl->Bind(wxEVT_SPINCTRLDOUBLE,
                    [this, &component, val, index](wxSpinDoubleEvent& evt)
                    {
                        if (index < val->size())
                        {
                            (*val)[index] = static_cast<float>(evt.GetValue());
                            reInitComponent(component);
                        }
                    });

    if (canDelete)
    {
        wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
        removeButton->SetToolTip("Remove element");
        elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

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
    }

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateFileItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                           std::vector<GameEngine::File>* val, size_t index, std::function<void()> rebuildUI,
                                           bool canDelete)
{
    auto& editedFile    = (*val)[index];
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    auto row = CreateBrowseFileRow(pane, "File:", editedFile.GetDataRelativeDir());
    elemRow->Add(row.row, 1, wxEXPAND | wxRIGHT, 5);

    row.textCtrl->SetToolTip(row.textCtrl->GetValue());
    row.browseBtn->Bind(wxEVT_BUTTON,
                        [this, &component, txt = row.textCtrl, pane, &editedFile](wxCommandEvent& evt)
                        {
                            this->browseFileControlAction(evt, component, txt, pane, &editedFile);
                            txt->SetToolTip(txt->GetValue());
                        });

    row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                       [this, &component, &editedFile, txt = row.textCtrl](wxCommandEvent& evt)
                       {
                           editedFile = GameEngine::File(evt.GetString().ToStdString());
                           reInitComponent(component);
                           txt->SetToolTip(txt->GetValue());
                           evt.Skip();
                       });
    row.textCtrl->SetDropTarget(new MyFileDropTarget(row.textCtrl,
                                                     [this, &component, &editedFile](const std::string& path)
                                                     {
                                                         editedFile = GameEngine::File(path);
                                                         reInitComponent(component);
                                                     }));

    if (canDelete)
    {
        wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
        removeButton->SetToolTip("Remove element");
        elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

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
    }

    return elemRow;
}

wxBoxSizer* ComponentPanel::CreateTextureItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                              std::vector<GameEngine::File>* val, size_t index, std::function<void()> rebuildUI,
                                              bool canDelete)
{
    auto& editedFile    = (*val)[index];
    wxBoxSizer* elemRow = new wxBoxSizer(wxHORIZONTAL);

    auto row = CreateBrowseTextureRow(pane, "Texture:", editedFile.GetDataRelativeDir());
    elemRow->Add(row.row, 1, wxEXPAND | wxRIGHT, 5);

    SetPreviewBitmap(row.preview, editedFile, pane);

    row.textCtrl->SetToolTip(row.textCtrl->GetValue());
    row.browseBtn->Bind(
        wxEVT_BUTTON,
        [this, &component, tr = row.textCtrl, prev = row.preview, pane, &editedFile, textCtrl = row.textCtrl](wxCommandEvent&)
        {
            wxFileDialog openFileDialog(pane, "Choose texture", EngineConf.files.data, "",
                                        "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (openFileDialog.ShowModal() == wxID_OK)
            {
                wxString path = openFileDialog.GetPath();
                editedFile = GameEngine::File(path.ToStdString());
                tr->SetValue(editedFile.GetDataRelativeDir());
                textCtrl->SetToolTip(path.ToStdString());
                reInitComponent(component);
                SetPreviewBitmap(prev, editedFile, pane);
            }
        });

    row.textCtrl->Bind(
        wxEVT_TEXT_ENTER,
        [this, &component, &editedFile, prev = row.preview, pane, textCtrl = row.textCtrl](wxCommandEvent& evt) mutable
        {
            editedFile = GameEngine::File(evt.GetString().ToStdString());
            SetPreviewBitmap(prev, editedFile, pane);
            textCtrl->SetToolTip(evt.GetString().ToStdString());
            reInitComponent(component);
            evt.Skip();
        });

    row.textCtrl->SetDropTarget(
        new MyFileDropTarget(row.textCtrl,
                             [this, &component, &editedFile, prev = row.preview, pane](const std::string& path)
                             {
                                 editedFile = GameEngine::File(path);
                                 SetPreviewBitmap(prev, editedFile, pane);
                                 reInitComponent(component);
                             }));

    if (canDelete)
    {
        wxButton* removeButton = new wxButton(pane, wxID_ANY, "Delete");
        removeButton->SetToolTip("Remove element");
        elemRow->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL);

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
    }

    return elemRow;
}

void ComponentPanel::reInitComponent(GameEngine::Components::IComponent& component)
{
    component.CleanUp();
    this->componentController.CallGameObjectFunctions(GameEngine::Components::FunctionType::Awake, gameObject.GetId());
}

void ComponentPanel::browseFileControlAction(wxCommandEvent&, GameEngine::Components::IComponent& component, wxTextCtrl* fileCtrl,
                                             wxWindow* pane, GameEngine::File* val)
{
    wxFileDialog openFileDialog(pane, "Choose file", EngineConf.files.data, "", "*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_OK)
    {
        fileCtrl->SetValue(GameEngine::File(openFileDialog.GetPath()).GetDataRelativeDir());
        fileCtrl->SetToolTip(openFileDialog.GetPath());
        val->Change(openFileDialog.GetPath().ToStdString());
        reInitComponent(component);
    }
}

wxBoxSizer* ComponentPanel::CreateLabeledRow(wxWindow* parent, const wxString& label, wxWindow* control, int proportion,
                                             int flags, int border)
{
    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
    row->Add(new wxStaticText(parent, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    row->Add(control, proportion, flags, border);
    return row;
}

wxTextCtrl* ComponentPanel::createTextEnterCtrl(wxWindow* pane, const std::string& text)
{
    auto result = new wxTextCtrl(pane, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    result->SetToolTip(text);
    return result;
}

ComponentPanel::BrowseRow ComponentPanel::CreateBrowseFileRow(wxWindow* parent, const wxString& label, const wxString& initial)
{
    BrowseRow out{};
    out.row = new wxBoxSizer(wxHORIZONTAL);
    out.row->Add(new wxStaticText(parent, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    out.textCtrl = createTextEnterCtrl(parent, initial.ToStdString());
    out.textCtrl->SetToolTip(initial);
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
    out.textCtrl->SetToolTip(initial);
    out.row->Add(out.textCtrl, 1, wxALIGN_CENTER_VERTICAL);

    out.browseBtn = new wxButton(parent, wxID_ANY, "Browse");
    out.row->Add(out.browseBtn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    out.preview =
        new wxStaticBitmap(parent, wxID_ANY, ThumbnailCache::Get().GetThumbnail(initial, 64), wxDefaultPosition, wxSize(64, 64));
    out.row->Add(out.preview, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 10);

    return out;
}

void ComponentPanel::SetPreviewBitmap(wxStaticBitmap* preview, const GameEngine::File& file, wxWindow* relayoutParent)
{
    if (file.exist())
    {
        SetPreviewBitmap(preview, wxString(file.GetAbsoultePath()), relayoutParent);
    }
}

void ComponentPanel::SetPreviewBitmap(wxStaticBitmap* preview, const wxString& path, wxWindow* relayoutParent)
{
    auto bmp = ThumbnailCache::Get().GetThumbnail(path, 64);
    if (bmp.IsOk())
    {
        preview->SetBitmap(bmp);
        if (relayoutParent)
            relayoutParent->Layout();
    }
}

wxBoxSizer* ComponentPanel::CreateUIForAnimationClip(GameEngine::Components::IComponent& component, wxWindow* pane,
                                                     GameEngine::Components::ReadAnimationInfo* val)
{
    wxBoxSizer* clipSizer = new wxBoxSizer(wxVERTICAL);

    // name
    {
        wxTextCtrl* ctrl = createTextEnterCtrl(pane, val->name);
        wxBoxSizer* row  = CreateLabeledRow(pane, "Name", ctrl);
        clipSizer->Add(row, 0, wxEXPAND | wxALL, 2);

        ctrl->Bind(wxEVT_TEXT_ENTER,
                   [this, &component, val, ctrl](wxCommandEvent& evt)
                   {
                       val->name = evt.GetString().ToStdString();
                       reInitComponent(component);
                       ctrl->SetToolTip(val->name);
                   });
    }

    // file
    {
        auto row = CreateBrowseFileRow(pane, "File", val->file.GetDataRelativeDir());
        clipSizer->Add(row.row, 0, wxEXPAND | wxALL, 2);

        row.browseBtn->Bind(wxEVT_BUTTON, [this, &component, txt = row.textCtrl, pane, val](wxCommandEvent& evt)
                            { this->browseFileControlAction(evt, component, txt, pane, &val->file); });

        row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                           [this, &component, val, txt = row.textCtrl](wxCommandEvent& evt)
                           {
                               val->file.Change(evt.GetString().ToStdString());
                               reInitComponent(component);
                               txt->SetToolTip(txt->GetValue());
                           });
        row.textCtrl->SetDropTarget(new MyFileDropTarget(row.textCtrl,
                                                         [this, &component, val](const std::string& path)
                                                         {
                                                             val->file.Change(path);
                                                             reInitComponent(component);
                                                         }));
    }

    // playInLoop
    {
        wxCheckBox* check = new wxCheckBox(pane, wxID_ANY, "Play In Loop");
        check->SetValue(val->playInLoop);
        clipSizer->Add(check, 0, wxALL, 2);

        check->Bind(wxEVT_CHECKBOX,
                    [this, &component, val](wxCommandEvent& e)
                    {
                        val->playInLoop = e.IsChecked();
                        reInitComponent(component);
                    });
    }

    // useRootMotion
    {
        wxCheckBox* check = new wxCheckBox(pane, wxID_ANY, "Use Root Motion");
        check->SetValue(val->useRootMontion);
        clipSizer->Add(check, 0, wxALL, 2);

        check->Bind(wxEVT_CHECKBOX,
                    [this, &component, val](wxCommandEvent& e)
                    {
                        val->useRootMontion = e.IsChecked();
                        reInitComponent(component);
                    });
    }

    // playSpeed
    {
        auto ctrl = CreateFloatSpinCtrl(pane, val->playSpeed, 0.01, 10.0, 0.1, 2);
        auto row  = CreateLabeledRow(pane, "Play Speed", ctrl);
        clipSizer->Add(row, 0, wxEXPAND | wxALL, 2);

        ctrl->Bind(wxEVT_SPINCTRLDOUBLE,
                   [this, &component, val](auto& evt)
                   {
                       val->playSpeed = static_cast<float>(evt.GetValue());
                       reInitComponent(component);
                   });
    }

    return clipSizer;
}

wxBoxSizer* ComponentPanel::CreateAnimationClipItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                                    std::vector<GameEngine::Components::ReadAnimationInfo>* val, size_t index,
                                                    std::function<void()> rebuildUI, bool canDelete)
{
    wxStaticBoxSizer* box = new wxStaticBoxSizer(wxVERTICAL, pane, "AnimationClip " + std::to_string(index));

    if (index >= val->size())
        return box;

    auto& clip = (*val)[index];

    // Wykorzystujemy wspólną funkcję do tworzenia pól AnimationClip
    auto clipSizer = CreateUIForAnimationClip(component, pane, &clip);
    box->Add(clipSizer, 0, wxEXPAND | wxALL, 2);

    // przycisk usuwania
    if (canDelete)
    {
        auto removeButton = new wxButton(pane, wxID_ANY, "Delete");
        removeButton->SetToolTip("Remove element");
        box->Add(removeButton, 0, wxALIGN_RIGHT | wxALL, 2);

        removeButton->Bind(wxEVT_BUTTON,
                           [this, &component, val, index, rebuildUI](auto&)
                           {
                               if (index < val->size())
                               {
                                   val->erase(val->begin() + index);
                                   reInitComponent(component);
                                   this->CallAfter(rebuildUI);
                               }
                           });
    }

    return box;
}

wxSpinCtrlDouble* ComponentPanel::CreateFloatSpinCtrl(wxWindow* parent, double value, double min, double max, double inc,
                                                      int digits)
{
    auto* ctrl = new wxSpinCtrlDouble(parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
    ctrl->SetRange(min, max);
    ctrl->SetDigits(digits);
    ctrl->SetIncrement(inc);
    ctrl->SetValue(value);
    ctrl->Bind(wxEVT_MOUSEWHEEL, [](auto& evt) {});

    return ctrl;
}
