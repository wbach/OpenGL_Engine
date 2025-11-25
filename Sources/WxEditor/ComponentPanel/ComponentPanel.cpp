#include "ComponentPanel.h"

#include <GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h>
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Engine/ExternalComponentsReader.h>
#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Resources/File.h>
#include <Logger/Log.h>
#include <wx/artprov.h>
#include <wx/dnd.h>
#include <wx/wx.h>

#include <magic_enum/magic_enum.hpp>
#include <mutex>
#include <utility>

#include "ReloadComponentLibEvent.h"
#include "WxEditor/WxHelpers/FileDropTarget.h"
#include "WxEditor/WxHelpers/ThumbnailCache.h"

namespace GameEngine
{
namespace Components
{
extern const std::string CSTR_TYPE;
}
}  // namespace GameEngine

ComponentPanel::ComponentPanel(wxFrame* mainFrame, wxWindow* parent, GameEngine::ExternalComponentsReader& reader,
                               GameEngine::GameObject& gameObject)
    : wxPanel(parent, wxID_ANY)
    , mainFrame(mainFrame)
    , externalComponentsReader(reader)
    , gameObject{gameObject}
{
    mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);
}

void ComponentPanel::Lock()
{
    std::lock_guard<std::mutex> lk(mutex);
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
    std::lock_guard<std::mutex> lk(mutex);
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
    std::lock_guard<std::mutex> lk(mutex);
    mainSizer->Clear(true);
    Layout();
}

void ComponentPanel::AddComponent(GameEngine::Components::IComponent& component, bool collapsed)
{
    std::lock_guard<std::mutex> lk(mutex);

    const auto& typeName = component.GetTypeName();

    headerPanel             = new wxPanel(this);
    wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

    // Label z nazwa sekcji
    wxStaticText* title = new wxStaticText(headerPanel, wxID_ANY, "Component name: " + typeName);
    headerSizer->Add(title, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);

    // Checkbox aktywnosci
    {
        auto* activeCheck = new wxCheckBox(headerPanel, wxID_ANY, "Active");
        activeCheck->SetValue(component.IsActive());
        headerSizer->Add(activeCheck, 0, wxALL, 5);

        activeCheck->Bind(wxEVT_CHECKBOX,
                          [&component](auto& e)
                          {
                              component.SetActive(e.IsChecked());
                              component.Reload();
                          });
    }

    auto externalLoadedLibs = externalComponentsReader.GetLoadedLibs();
    for (auto& [file, name] : externalLoadedLibs)
    {
        if (name == component.GetTypeName())
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
                                [this, &component, typeName](const auto&)
                                {
                                    int answer = wxMessageBox("Delete component " + typeName + "?", "Confirmation",
                                                              wxYES_NO | wxICON_QUESTION);
                                    if (answer == wxYES)
                                    {
                                        CallAfter(
                                            [this, &component]()
                                            {
                                                gameObject.RemoveComponent(component);
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

    // Sizer dla zawartosci collapsible
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    pane->SetSizer(sizer);

    CreateUIForComponent(component, pane, sizer);

    pane->Layout();
    sizer->Layout();

    // Dodajemy do glownego sizer
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
    // Aktualizacja layoutu po zwinieciu/rozwinieciu
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
                       [&component, ptr = field.ptr, indexToEnum = field.indexToEnum](wxCommandEvent& evt)
                       {
                           LOG_DEBUG << "OnChoice";
                           indexToEnum(ptr, evt.GetSelection());
                           LOG_DEBUG << "reInit";
                           component.Reload();
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
                component, pane, sizer, field, [this, &component](auto p, auto v, auto i, auto r, auto del)
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
                       [&component, val](auto& evt)
                       {
                           *val = evt.GetValue();
                           component.Reload();
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
                       [&component, val](auto& evt)
                       {
                           *val = static_cast<float>(evt.GetValue());
                           component.Reload();
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
                       [&component, val](auto& evt)
                       {
                           *val = evt.GetString().ToStdString();
                           component.Reload();
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
                        [val, &component](auto& e)
                        {
                            *val = e.IsChecked();
                            component.Reload();
                        });
            break;
        }

        case FieldType::File:
        {
            auto* val = static_cast<GameEngine::File*>(field.ptr);
            auto row  = CreateBrowseFileRow(pane, field.name, val->GetDataRelativePath().string());
            sizer->Add(row.row, 0, wxEXPAND | wxALL, 5);
            row.textCtrl->SetToolTip(row.textCtrl->GetValue());
            // Browse action
            row.browseBtn->Bind(wxEVT_BUTTON,
                                [this, &component, txt = row.textCtrl, pane, val, warningIcon = row.warningIcon](auto& evt)
                                {
                                    this->browseFileControlAction(evt, component, txt, pane, val);
                                    UpdateFileWarning(warningIcon, val->GetAbsolutePath());
                                });

            // Enter w polu
            row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                               [this, &component, val, txt = row.textCtrl, warningIcon = row.warningIcon](auto& evt)
                               {
                                   val->Init(evt.GetString().ToStdString());
                                   component.Reload();
                                   txt->SetToolTip(txt->GetValue());
                                   UpdateFileWarning(warningIcon, val->GetAbsolutePath());
                               });

            row.textCtrl->SetDropTarget(new FileDropTarget(
                [this, &component, val, warningIcon = row.warningIcon, ctrl = row.textCtrl](const std::string& path)
                {
                    ctrl->ChangeValue(GameEngine::File(path).GetDataRelativePath().string());
                    ctrl->SetToolTip(path);

                    val->Init(path);
                    component.Reload();
                    UpdateFileWarning(warningIcon, val->GetAbsolutePath());
                }));
            break;
        }

        case FieldType::Texture:
        {
            auto* val = static_cast<GameEngine::File*>(field.ptr);

            auto row = CreateBrowseTextureRow(pane, field.name, val->GetDataRelativePath().string());
            sizer->Add(row.row, 0, wxEXPAND | wxALL, 5);

            SetPreviewBitmap(row.preview, *val, pane);
            row.textCtrl->SetToolTip(row.textCtrl->GetValue());
            // Browse action
            row.browseBtn->Bind(wxEVT_BUTTON,
                                [this, &component, txt = row.textCtrl, prev = row.preview, pane, val,
                                 warningIcon = row.warningIcon](wxCommandEvent&)
                                {
                                    wxFileDialog openFileDialog(pane, "Choose texture", EngineConf.files.getDataPath().string(),
                                                                "", "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                                    if (openFileDialog.ShowModal() == wxID_OK)
                                    {
                                        wxString path = openFileDialog.GetPath();
                                        val->Init(path.ToStdString());
                                        txt->SetValue(val->GetDataRelativePath().string());
                                        component.Reload();
                                        SetPreviewBitmap(prev, GameEngine::File{path.ToStdString()}, pane);
                                        txt->SetToolTip(txt->GetValue());
                                        UpdateFileWarning(warningIcon, val->GetAbsolutePath().string());
                                    }
                                });

            // Enter w polu
            row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                               [this, &component, val, prev = row.preview, pane, txt = row.textCtrl,
                                warningIcon = row.warningIcon](wxCommandEvent& evt)
                               {
                                   val->Init(evt.GetString().ToStdString());
                                   component.Reload();
                                   SetPreviewBitmap(prev, GameEngine::File{evt.GetString().ToStdString()}, pane);
                                   txt->SetToolTip(txt->GetValue());
                                   txt->SetValue(val->GetDataRelativePath().string());
                                   UpdateFileWarning(warningIcon, val->GetAbsolutePath().string());
                               });

            row.textCtrl->SetDropTarget(new FileDropTarget(
                [this, &component, pane, val, warningIcon = row.warningIcon, prev = row.preview,
                 ctrl = row.textCtrl](const std::string& path)
                {
                    ctrl->ChangeValue(GameEngine::File(path).GetDataRelativePath().string());
                    ctrl->SetToolTip(path);

                    val->Init(path);
                    component.Reload();
                    SetPreviewBitmap(prev, *val, pane);
                    UpdateFileWarning(warningIcon, val->GetAbsolutePath());
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
        case GameEngine::Components::FieldType::VectorOfTerrainTextures:
        {
            CreateUIForVector<GameEngine::Components::TerrainTexture>(
                component, pane, sizer, field,
                [this, &component](wxWindow* parent, std::vector<GameEngine::Components::TerrainTexture>* vec, size_t index,
                                   std::function<void()> onRemove, bool editable)
                { return this->CreateTerrainTextureItem(component, parent, vec, index, onRemove, editable); });
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
                [](wxSpinDoubleEvent& e) { return static_cast<float>(e.GetValue()); }, [](wxSpinCtrlDouble* ctrl, auto handler)
                { ctrl->Bind(wxEVT_SPINCTRLDOUBLE, handler); }, {"R:", "G:", "B:", "A:"});
            break;
        }
        // == Wektory ==
        case FieldType::VectorOfStrings:
            CreateUIForVector<std::string>(component, pane, sizer, field,
                                           [this, &component](auto p, auto v, auto i, auto r, auto del)
                                           { return this->CreateStringItem(component, p, v, i, r, del); });
            break;

        case FieldType::VectorOfInt:
            CreateUIForVector<int>(component, pane, sizer, field, [this, &component](auto p, auto v, auto i, auto r, auto del)
                                   { return this->CreateIntItem(component, p, v, i, r, del); });
            break;

        case FieldType::VectorOfFloat:
            CreateUIForVector<float>(component, pane, sizer, field, [this, &component](auto p, auto v, auto i, auto r, auto del)
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
                component, pane, sizer, field, [this, &component](auto p, auto v, auto i, auto r, auto del)
                { return this->CreateTextureItem(component, p, v, i, r, del); }, false);
            break;

        case FieldType::ConstMapOfMaterials:
            auto* materials = static_cast<GameEngine::MaterialsMap*>(field.ptr);
            CreateUIForMaterialsMap(component, pane, sizer, *materials);
            break;
    }
}

void ComponentPanel::CreateUIForMaterialsMap(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                             GameEngine::MaterialsMap& materials)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ---- Label sekcji ----
    wxStaticText* sectionLabel = new wxStaticText(pane, wxID_ANY, "Materials");
    sectionLabel->SetFont(sectionLabel->GetFont().MakeBold());  // pogrubienie
    mainSizer->Add(sectionLabel, 0, wxBOTTOM | wxEXPAND, 5);

    // ---- Elementy mapy ----
    for (auto it = materials.begin(); it != materials.end(); ++it)
    {
        auto row = CreateMaterialMapItem(component, pane, it, []() {});
        mainSizer->Add(row, 0, wxEXPAND | wxBOTTOM, 3);
    }

    sizer->Add(mainSizer, 0, wxEXPAND | wxALL, 5);
}

wxBoxSizer* ComponentPanel::CreateMaterialMapItem(GameEngine::Components::IComponent& component, wxWindow* pane,
                                                  GameEngine::MaterialsMap::iterator it, std::function<void()> rebuildUI)
{
    wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);

    // Klucz - nazwa materialu (readonly)
    wxTextCtrl* nameCtrl =
        new wxTextCtrl(pane, wxID_ANY, wxString::FromUTF8(it->first.c_str()), wxDefaultPosition, wxSize(150, -1), wxTE_READONLY);
    nameCtrl->Enable(false);  // wyszarzone
    row->Add(nameCtrl, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    // Wartosc - File
    auto initialValue = it->second.empty() ? "default" : it->second.GetDataRelativePath().string();
    auto rowFile      = CreateBrowseFileRow(pane, "File", initialValue);
    row->Add(rowFile.row, 1, wxEXPAND | wxALL, 0);
    rowFile.textCtrl->SetToolTip(rowFile.textCtrl->GetValue());

    // Browse button
    rowFile.browseBtn->Bind(
        wxEVT_BUTTON,
        [this, &component, txt = rowFile.textCtrl, val = &it->second, pane, warningIcon = rowFile.warningIcon](auto& evt)
        {
            this->browseFileControlAction(evt, component, txt, pane, val);
            UpdateFileWarning(warningIcon, val->GetAbsolutePath());
        });

    // Text enter
    rowFile.textCtrl->Bind(
        wxEVT_TEXT_ENTER,
        [this, &component, val = &it->second, txt = rowFile.textCtrl, warningIcon = rowFile.warningIcon](auto& evt)
        {
            val->Init(evt.GetString().ToStdString());
            component.Reload();
            txt->SetToolTip(txt->GetValue());
            UpdateFileWarning(warningIcon, val->GetAbsolutePath());
        });

    // Drag & drop
    rowFile.textCtrl->SetDropTarget(new FileDropTarget(
        [this, &component, val = &it->second, warningIcon = rowFile.warningIcon, ctrl = rowFile.textCtrl](const std::string& path)
        {
            ctrl->ChangeValue(GameEngine::File(path).GetDataRelativePath().string());
            ctrl->SetToolTip(path);

            val->Init(path);
            component.Reload();
            UpdateFileWarning(warningIcon, val->GetAbsolutePath());
        }));

    return row;
}

template <typename VecT, typename CtrlT, typename BindEvt, typename SetVal, typename GetVal>
void ComponentPanel::CreateVectorRow(GameEngine::Components::IComponent& component, wxWindow* pane, wxBoxSizer* sizer,
                                     const wxString& label, VecT* vec, int dimensions, SetVal setCtrlValue, GetVal getCtrlValue,
                                     BindEvt bindEvent, const std::vector<std::string>& axisNames)
{
    // Etykieta nad polami
    sizer->Add(new wxStaticText(pane, wxID_ANY, label), 0, wxBOTTOM, 2);

    // Wlasny panel, zeby odseparowac layout od reszty
    wxPanel* rowPanel    = new wxPanel(pane);
    wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);

    for (int i = 0; i < dimensions; ++i)
    {
        // Os
        rowSizer->Add(new wxStaticText(rowPanel, wxID_ANY, axisNames[i]), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        // Pole - wlasny startowy rozmiar
        CtrlT* ctrl = new CtrlT(rowPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS);
        setCtrlValue(ctrl, (*vec)[i]);

        // Rozciaganie tylko w obrebie tego wiersza
        rowSizer->Add(ctrl, 1, wxRIGHT, (i == dimensions - 1 ? 0 : 10));

        bindEvent(ctrl,
                  [&component, vec, i, getCtrlValue](auto& evt)
                  {
                      (*vec)[i] = getCtrlValue(evt);
                      component.Reload();
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
        component.Reload();

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
                           {
                               LOG_WARN << "Parse error";
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
                     [&component, val, index, stringCtrl](wxCommandEvent& evt)
                     {
                         if (index < val->size())
                         {
                             (*val)[index] = evt.GetString().ToStdString();
                             stringCtrl->SetToolTip(evt.GetString().ToStdString());
                             component.Reload();
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
                                   component.Reload();
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
                   [&component, val, index](wxSpinEvent& evt)
                   {
                       if (index < val->size())
                       {
                           (*val)[index] = evt.GetPosition();
                           component.Reload();
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
                                   component.Reload();
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
                    [&component, val, index](wxSpinDoubleEvent& evt)
                    {
                        if (index < val->size())
                        {
                            (*val)[index] = static_cast<float>(evt.GetValue());
                            component.Reload();
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
                                   component.Reload();
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

    auto row = CreateBrowseFileRow(pane, "File:", editedFile.GetDataRelativePath().string());
    elemRow->Add(row.row, 1, wxEXPAND | wxRIGHT, 5);

    row.textCtrl->SetToolTip(row.textCtrl->GetValue());
    row.browseBtn->Bind(
        wxEVT_BUTTON,
        [this, &component, txt = row.textCtrl, pane, &editedFile, warningIcon = row.warningIcon](wxCommandEvent& evt)
        {
            this->browseFileControlAction(evt, component, txt, pane, &editedFile);
            txt->SetToolTip(txt->GetValue());
            UpdateFileWarning(warningIcon, txt->GetValue().ToStdString());
        });

    row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                       [this, &component, &editedFile, txt = row.textCtrl, warningIcon = row.warningIcon](wxCommandEvent& evt)
                       {
                           editedFile = GameEngine::File(evt.GetString().ToStdString());
                           component.Reload();
                           txt->SetToolTip(txt->GetValue());
                           UpdateFileWarning(warningIcon, txt->GetValue().ToStdString());
                           evt.Skip();
                       });
    row.textCtrl->SetDropTarget(new FileDropTarget(
        [this, &component, &editedFile, warningIcon = row.warningIcon, ctrl = row.textCtrl](const std::string& path)
        {
            ctrl->ChangeValue(GameEngine::File(path).GetDataRelativePath().string());
            ctrl->SetToolTip(path);

            editedFile = GameEngine::File(path);
            UpdateFileWarning(warningIcon, path);
            component.Reload();
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
                                   component.Reload();
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

    auto row = CreateBrowseTextureRow(pane, "Texture:", editedFile.GetDataRelativePath().string());
    elemRow->Add(row.row, 1, wxEXPAND | wxRIGHT, 5);

    SetPreviewBitmap(row.preview, editedFile, pane);

    row.textCtrl->SetToolTip(row.textCtrl->GetValue());
    row.browseBtn->Bind(wxEVT_BUTTON,
                        [this, &component, tr = row.textCtrl, prev = row.preview, pane, &editedFile, textCtrl = row.textCtrl,
                         warningIcon = row.warningIcon](wxCommandEvent&)
                        {
                            wxFileDialog openFileDialog(pane, "Choose texture", EngineConf.files.getDataPath().string(), "",
                                                        "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                            if (openFileDialog.ShowModal() == wxID_OK)
                            {
                                wxString path = openFileDialog.GetPath();
                                editedFile    = GameEngine::File(path.ToStdString());
                                tr->SetValue(editedFile.GetDataRelativePath().string());
                                textCtrl->SetToolTip(path.ToStdString());
                                component.Reload();
                                SetPreviewBitmap(prev, editedFile, pane);
                                UpdateFileWarning(warningIcon, editedFile.GetAbsolutePath());
                            }
                        });

    row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                       [this, &component, &editedFile, prev = row.preview, pane, textCtrl = row.textCtrl,
                        warningIcon = row.warningIcon](wxCommandEvent& evt) mutable
                       {
                           editedFile = GameEngine::File(evt.GetString().ToStdString());
                           SetPreviewBitmap(prev, editedFile, pane);
                           textCtrl->SetToolTip(evt.GetString().ToStdString());
                           component.Reload();
                           UpdateFileWarning(warningIcon, editedFile.GetAbsolutePath());
                           evt.Skip();
                       });

    row.textCtrl->SetDropTarget(new FileDropTarget(
        [this, &component, &editedFile, prev = row.preview, pane, warningIcon = row.warningIcon,
         ctrl = row.textCtrl](const std::string& path)
        {
            ctrl->ChangeValue(GameEngine::File(path).GetDataRelativePath().string());
            ctrl->SetToolTip(path);

            editedFile = GameEngine::File(path);
            SetPreviewBitmap(prev, editedFile, pane);
            component.Reload();
            UpdateFileWarning(warningIcon, editedFile.GetAbsolutePath());
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
                                   component.Reload();
                                   this->CallAfter(rebuildUI);
                               }
                           });
    }

    return elemRow;
}

void ComponentPanel::browseFileControlAction(wxCommandEvent&, GameEngine::Components::IComponent& component, wxTextCtrl* fileCtrl,
                                             wxWindow* pane, GameEngine::File* val)
{
    wxFileDialog openFileDialog(pane, "Choose file", EngineConf.files.getDataPath().string(), "", "*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_OK)
    {
        fileCtrl->SetValue(GameEngine::File(openFileDialog.GetPath()).GetDataRelativePath().string());
        fileCtrl->SetToolTip(openFileDialog.GetPath());
        val->Init(openFileDialog.GetPath().ToStdString());
        component.Reload();
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

    out.warningIcon = new wxStaticBitmap(parent, wxID_ANY, wxBitmap(16, 16));
    out.row->Add(out.warningIcon, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    out.warningIcon->Hide();

    UpdateFileWarning(out.warningIcon, initial.ToStdString());

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

    out.warningIcon = new wxStaticBitmap(parent, wxID_ANY, wxBitmap(16, 16));
    out.row->Add(out.warningIcon, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    out.warningIcon->Hide();

    out.preview =
        new wxStaticBitmap(parent, wxID_ANY, ThumbnailCache::Get().GetThumbnail(initial, 64), wxDefaultPosition, wxSize(64, 64));
    out.row->Add(out.preview, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 10);

    UpdateFileWarning(out.warningIcon, initial.ToStdString());

    return out;
}

void ComponentPanel::SetPreviewBitmap(wxStaticBitmap* preview, const GameEngine::File& file, wxWindow* relayoutParent)
{
    if (file.exist())
    {
        SetPreviewBitmap(preview, wxString(file.GetAbsolutePath()), relayoutParent);
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
                       component.Reload();
                       ctrl->SetToolTip(val->name);
                   });
    }

    // file
    {
        auto row = CreateBrowseFileRow(pane, "File", val->file.GetDataRelativePath().string());
        clipSizer->Add(row.row, 0, wxEXPAND | wxALL, 2);

        row.browseBtn->Bind(wxEVT_BUTTON,
                            [this, &component, txt = row.textCtrl, pane, val, warningIcon = row.warningIcon](wxCommandEvent& evt)
                            {
                                this->browseFileControlAction(evt, component, txt, pane, &val->file);
                                UpdateFileWarning(warningIcon, val->file.GetAbsolutePath());
                            });

        row.textCtrl->Bind(wxEVT_TEXT_ENTER,
                           [this, &component, val, txt = row.textCtrl, warningIcon = row.warningIcon](wxCommandEvent& evt)
                           {
                               val->file.Init(evt.GetString().ToStdString());
                               component.Reload();
                               txt->SetToolTip(txt->GetValue());
                               UpdateFileWarning(warningIcon, val->file.GetAbsolutePath());
                           });
        row.textCtrl->SetDropTarget(new FileDropTarget(
            [this, &component, val, warningIcon = row.warningIcon, ctrl = row.textCtrl](const std::string& path)
            {
                ctrl->ChangeValue(GameEngine::File(path).GetDataRelativePath().string());
                ctrl->SetToolTip(path);

                val->file.Init(path);
                component.Reload();
                UpdateFileWarning(warningIcon, val->file.GetAbsolutePath().string());
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
                        component.Reload();
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
                        component.Reload();
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
                       component.Reload();
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

    // Wykorzystujemy wspolna funkcje do tworzenia pol AnimationClip
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
                                   component.Reload();
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

void ComponentPanel::UpdateFileWarning(wxStaticBitmap* warningIcon, const GameEngine::File& file)
{
    if (file.exist())
    {
        warningIcon->Hide();
    }
    else
    {
        // Zakladamy, ze masz ikone wykrzyknika w zasobach lub pliku
        auto bmp = wxArtProvider::GetBitmap(wxART_ERROR, wxART_OTHER, wxSize(16, 16));
        warningIcon->SetBitmap(bmp);
        warningIcon->Show();
    }
    if (warningIcon->GetParent())
        warningIcon->GetParent()->Layout();
}

wxBoxSizer* ComponentPanel::CreateTerrainTextureItem(GameEngine::Components::IComponent& component, wxWindow* parent,
                                                     std::vector<GameEngine::Components::TerrainTexture>* vec, size_t index,
                                                     std::function<void()> onRemove, bool editable)
{
    using namespace GameEngine::Components;

    auto* itemSizer  = new wxBoxSizer(wxVERTICAL);
    auto& terrainTex = (*vec)[index];

    // --- Texture file ---
    auto texRow = CreateBrowseTextureRow(parent, "Texture", terrainTex.file.GetDataRelativePath().string());
    itemSizer->Add(texRow.row, 0, wxEXPAND | wxALL, 3);
    SetPreviewBitmap(texRow.preview, terrainTex.file, parent);
    UpdateFileWarning(texRow.warningIcon, terrainTex.file.GetAbsolutePath());

    // Browse button
    texRow.browseBtn->Bind(wxEVT_BUTTON,
                           [this, &component, parent, &terrainTex, txt = texRow.textCtrl, prev = texRow.preview,
                            warn = texRow.warningIcon](wxCommandEvent&)
                           {
                               wxFileDialog openFileDialog(parent, "Choose texture", EngineConf.files.getDataPath().string(), "",
                                                           "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                                           wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                               if (openFileDialog.ShowModal() == wxID_OK)
                               {
                                   wxString path = openFileDialog.GetPath();
                                   terrainTex.file.Init(path.ToStdString());
                                   txt->SetValue(terrainTex.file.GetDataRelativePath().string());
                                   txt->SetToolTip(txt->GetValue());
                                   SetPreviewBitmap(prev, terrainTex.file, parent);
                                   UpdateFileWarning(warn, terrainTex.file.GetAbsolutePath());
                                   component.Reload();
                               }
                           });

    if (terrainTex.type != GameEngine::TerrainTextureType::blendMap and
        terrainTex.type != GameEngine::TerrainTextureType::heightmap)
    {
        // --- Tiled Scale ---
        auto* scaleCtrl = CreateFloatSpinCtrl(parent, terrainTex.tiledScale, 0.01, 1000.0, 0.1, 3);
        auto* scaleRow  = CreateLabeledRow(parent, "Tiled Scale", scaleCtrl);
        itemSizer->Add(scaleRow, 0, wxEXPAND | wxALL, 3);

        scaleCtrl->Bind(wxEVT_SPINCTRLDOUBLE,
                        [&component, &terrainTex](wxSpinDoubleEvent& evt)
                        {
                            terrainTex.tiledScale = static_cast<float>(evt.GetValue());
                            component.Reload();
                        });
    }

    // --- Type (enum) ---
    auto enumValues = magic_enum::enum_values<GameEngine::TerrainTextureType>();

    // Wypelnij kontrolke nazwami (kolejnosc odpowiada enumValues)
    auto* typeCtrl = new wxChoice(parent, wxID_ANY);
    for (auto v : enumValues)
    {
        typeCtrl->Append(wxString::FromUTF8(std::string(magic_enum::enum_name(v)).c_str()));
    }

    // Znajdz indeks odpowiadajacy aktualnej wartosci terrainTex.type
    size_t selectedIndex = 0;
    for (size_t i = 0; i < enumValues.size(); ++i)
    {
        if (enumValues[i] == terrainTex.type)
        {
            selectedIndex = i;
            break;
        }
    }
    typeCtrl->SetSelection(static_cast<int>(selectedIndex));

    // Po wybraniu pozycji ustaw enumerator z enumValues wg indeksu
    auto* typeRow = CreateLabeledRow(parent, "Type", typeCtrl);
    itemSizer->Add(typeRow, 0, wxEXPAND | wxALL, 3);

    typeCtrl->Bind(wxEVT_CHOICE,
                   [this, &component, &terrainTex, enumValues](wxCommandEvent& evt)
                   {
                       auto sel = static_cast<size_t>(evt.GetSelection());
                       if (sel < enumValues.size())
                       {
                           terrainTex.type = enumValues[sel];
                           component.Reload();
                       }
                   });

    // --- Remove button if editable ---
    if (editable)
    {
        auto* delBtn = new wxButton(parent, wxID_ANY, "Remove");
        delBtn->Bind(wxEVT_BUTTON, [onRemove](auto&) { onRemove(); });
        itemSizer->Add(delBtn, 0, wxALIGN_RIGHT | wxALL, 3);
    }

    return itemSizer;
}
