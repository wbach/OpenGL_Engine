#include "GuiEditorFrame.h"

#include <GameEngine/Renderers/GUI/Button/Button.h>
#include <GameEngine/Renderers/GUI/EditText/EditText.h>
#include <GameEngine/Renderers/GUI/Element.h>
#include <GameEngine/Renderers/GUI/ElementReader.h>
#include <GameEngine/Renderers/GUI/ElementWriter.h>
#include <GameEngine/Renderers/GUI/ElementsDef.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Sprite/Sprite.h>
#include <GameEngine/Renderers/GUI/Text/MultiLineText.h>
#include <GameEngine/Renderers/GUI/Text/Text.h>
#include <GameEngine/Renderers/GUI/VerticalAlign.h>
#include <GameEngine/Renderers/GUI/Window/Window.h>
#include <GameEngine/Renderers/GUI/Window/WindowStyle.h>
#include <Logger/Log.h>
#include <Types.h>
#include <wx/filedlg.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/props.h>

#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <unordered_map>
#include <vector>

#include "AddElementDialog.h"
#include "GuiEditorControlIds.h"
#include "GuiTreeItemData.h"
#include "Renderers/GUI/IElementVisitor.h"
#include "Resources/FileHandle.h"
#include "TreeNode.h"
#include "WxEditor/EngineRelated/GLCanvas.h"
#include "WxEditor/EngineRelated/WxScenesDef.h"
#include "WxEditor/ProjectManager.h"

namespace
{
class FileHandleProperty : public wxStringProperty
{
public:
    FileHandleProperty(const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                       const wxString& value = wxEmptyString)
        : wxStringProperty(label, name, value)
    {
        SetEditor(static_cast<wxPGEditor*>(wxPGEditor_TextCtrlAndButton));
    }

    bool OnEvent(wxPropertyGrid* propgrid, wxWindow* primary, wxEvent& event) override
    {
        if (event.GetEventType() == wxEVT_BUTTON)
        {
            return HandleButtonClick(propgrid);
        }

        return wxStringProperty::OnEvent(propgrid, primary, event);
    }

    void OnSetValue() override
    {
        if (GetValue().GetString().StartsWith("mem://"))
            SetTextColour(*wxBLUE);
        else
            SetTextColour(*wxBLACK);
    }

private:
    bool HandleButtonClick(wxPropertyGrid* propgrid)
    {
        auto currentValue = GetValueAsString();
        wxString newValue;

        wxArrayString choices;
        choices.Add("File from disk (standard path)");
        choices.Add("Embedded resource (mem://)");

        int sel = wxGetSingleChoiceIndex("Select the asset source type:", "Asset Source", choices, propgrid);

        if (sel == 0)
        {
            wxFileDialog dlg(propgrid, "Select file", "", "", "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

            if (dlg.ShowModal() == wxID_OK)
            {
                newValue = dlg.GetPath();
            }
            else
                return false;
        }
        else if (sel == 1)
        {
            newValue = wxGetTextFromUser("Enter resource name (without mem:// prefix):", "Memory Resource",
                                         currentValue.StartsWith("mem://") ? currentValue.Mid(6) : "segoe-ui.ttf");

            if (newValue.IsEmpty())
                return false;
            newValue = "mem://" + newValue;
        }
        else
            return false;

        if (not newValue.IsEmpty() && newValue != GetValueAsString())
        {
            propgrid->ChangePropertyValue(this, newValue);
            return true;
        }

        SetValueFromString(newValue, wxPG_FULL_VALUE);

        return true;
    }
};

template <typename T>
wxPGChoices CreateChoicesFromEnum()
{
    wxPGChoices choices;
    for (auto const& [value, name] : magic_enum::enum_entries<T>())
    {
        choices.Add(wxString(name.data(), name.size()), static_cast<int>(value));
    }
    return choices;
}
void AppendColorProperty(wxPropertyGrid& pg, const Color& color, const wxString& label = "Color",
                         const wxString& propName = "ElementColor")
{
    wxColour wxC(color.r(), color.g(), color.b(), color.a());
    wxPGProperty* p = pg.Append(new wxColourProperty(label, propName, wxC));
    p->SetAttribute(wxPG_COLOUR_HAS_ALPHA, true);
}
Color ConvertVariantToColor(const wxVariant& value)
{
    wxColour col;
    if (value.IsType("wxColourPropertyValue"))
    {
        auto* cpv = static_cast<wxColourPropertyValue*>(value.GetVoidPtr());
        if (cpv)
        {
            col = cpv->m_colour;
        }
    }
    else
    {
        col << value;
    }

    if (not col.IsOk())
    {
        return Color(0);
    }
    return Color(col.Red(), col.Green(), col.Blue(), col.Alpha());
}

wxPGProperty* AppendProperty(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement, wxPGProperty* property)
{
    auto p = propGrid.Append(property);
    p->SetClientData(&selectedElement);
    return p;
}

const std::string LONG_DUMMY_TEXT{
    "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's "
    "standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to "
    "make a type specimen book. It has survived not only five centuries, but also the leap into electronic "
    "typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset "
    "sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker "
    "including versions of Lorem Ipsum."};
}  // namespace

GuiEditorFrame::GuiEditorFrame(const std::optional<GameEngine::File>& maybeFile, const wxString& title, const wxPoint& pos,
                               const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
    , currentFile(maybeFile)
{
    ReadRecentOpenedFiles();

    mainSplitter    = new wxSplitterWindow(this, wxID_ANY);
    leftPanel       = new wxPanel(mainSplitter);
    contentSplitter = new wxSplitterWindow(mainSplitter, wxID_ANY);

    auto onStartupDone = [this]() { Init(); };
    canvas             = new GLCanvas(contentSplitter, onStartupDone, nullptr, WxEditor::EMPTY_SCENE);

    propGrid = new wxPropertyGrid(contentSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                  wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE);

    contentSplitter->SplitVertically(canvas, propGrid, -300);
    mainSplitter->SplitVertically(leftPanel, contentSplitter, 250);

    if (not currentFile)
    {
        lastDirPath = ProjectManager::GetInstance().GetDataDir().string();
    }
    else
    {
        lastDirPath = currentFile->GetAbsolutePath().parent_path();
    }
}

void GuiEditorFrame::Init()
{
    auto* leftSizer = new wxBoxSizer(wxVERTICAL);
    sceneTree = new wxTreeCtrl(leftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HAS_BUTTONS);
    leftSizer->Add(new wxStaticText(leftPanel, wxID_ANY, "Hierarchy"), 0, wxALL | wxEXPAND, 5);
    leftSizer->Add(sceneTree, 1, wxEXPAND | wxALL, 2);
    leftPanel->SetSizer(leftSizer);
    leftPanel->Layout();

    sceneTree->Bind(wxEVT_TREE_SEL_CHANGED, &GuiEditorFrame::OnTreeSelectionChanged, this);
    propGrid->Bind(wxEVT_PG_CHANGED, &GuiEditorFrame::OnPropertyChange, this);

    CreateMainMenu();
    CreateStatusBar();

    timer = new wxTimer(this);
    Bind(wxEVT_TIMER, &GuiEditorFrame::OnTimer, this);
    timer->Start(16);

    RefreshTree();
}

void GuiEditorFrame::CreateMainMenu()
{
    auto* menuBar = new wxMenuBar();

    auto* fileMenu = new wxMenu();
    fileMenu->Append(wxID_OPEN, "&Open...\tCtrl+O");

    recentMenu = new wxMenu();
    UpdateRecentFilesMenu();
    fileMenu->AppendSubMenu(recentMenu, "Open &Recent");

    fileMenu->Append(ID_REOPEN, "Reopen");
    fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S");
    fileMenu->Append(wxID_SAVEAS, "Save &As...");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt+F4");

    auto* editMenu = new wxMenu();
    editMenu->Append(ID_UNDO, "&Undo\tCtrl+Z");
    editMenu->Append(ID_REDO, "&Redo\tCtrl+Y");
    editMenu->AppendSeparator();

    auto* addSubMenu = new wxMenu();
    addSubMenu->Append(ID_ADD_WINDOW, "Window");
    addSubMenu->Append(ID_ADD_TEXT, "Text Label");
    addSubMenu->Append(ID_ADD_MULTI_LINE_TEXT, "Multi line text");
    addSubMenu->Append(ID_ADD_TEXTURE, "Image/Texture");
    addSubMenu->Append(ID_ADD_BUTTON, "Button");
    addSubMenu->AppendSeparator();
    addSubMenu->Append(ID_ADD_V_LAYOUT, "Vertical Layout");
    addSubMenu->Append(ID_ADD_H_LAYOUT, "Horizontal Layout");

    editMenu->AppendSubMenu(addSubMenu, "&Add Element");

    menuBar->Append(fileMenu, "&File");
    menuBar->Append(editMenu, "&Edit");
    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, &GuiEditorFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &GuiEditorFrame::OnReOpen, this, ID_REOPEN);
    Bind(wxEVT_MENU, &GuiEditorFrame::OnSave, this, wxID_SAVE);
    Bind(wxEVT_MENU, &GuiEditorFrame::OnSaveAs, this, wxID_SAVEAS);
    Bind(wxEVT_MENU, &GuiEditorFrame::OnExit, this, wxID_EXIT);

    Bind(wxEVT_MENU, &GuiEditorFrame::OnUndo, this, ID_UNDO);
    Bind(wxEVT_MENU, &GuiEditorFrame::OnRedo, this, ID_REDO);

    Bind(wxEVT_MENU, &GuiEditorFrame::OnAddElement, this, ID_ADD_WINDOW, ID_ADD_H_LAYOUT);
    Bind(wxEVT_MENU, &GuiEditorFrame::OnOpenRecent, this, ID_RECENT_FIRST, ID_RECENT_LAST);

    sceneTree->Bind(wxEVT_TREE_ITEM_MENU, &GuiEditorFrame::OnTreeContextMenu, this);
}

void GuiEditorFrame::OnAddElement(wxCommandEvent& event)
{
    auto& factory = canvas->GetScene().GetGuiElementFactory();
    std::unique_ptr<GameEngine::GUI::Element> newElem;

    switch (event.GetId())
    {
        case ID_ADD_WINDOW:
            newElem = factory.createWindow(GameEngine::GUI::WindowStyle::BACKGROUND_ONLY);
            break;
        case ID_ADD_TEXT:
            newElem = factory.createText("New Label");
            break;
        case ID_ADD_MULTI_LINE_TEXT:
            newElem = factory.createMultiLineText(LONG_DUMMY_TEXT);
            break;
        case ID_ADD_TEXTURE:
            newElem = factory.createSprite("DefaultTexture.png");
            break;
        case ID_ADD_BUTTON:
            newElem = factory.createButton();
            break;
        case ID_ADD_V_LAYOUT:
            newElem = factory.createVerticalLayout();
            break;
        case ID_ADD_H_LAYOUT:
            newElem = factory.createHorizontalLayout();
            break;
    }

    if (newElem)
    {
        auto* ptrToSelect = newElem.get();
        canvas->GetScene().GetGuiManager().add(std::move(newElem));
        RefreshTree();
        FocusElementInTree(ptrToSelect);
        canvas->Refresh();
    }
}

void GuiEditorFrame::commonProperties(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement)
{
    AppendProperty(propGrid, selectedElement, new wxPropertyCategory("General"));
    AppendProperty(propGrid, selectedElement, new wxStringProperty("Label text", "ElementLabelText", selectedElement.getLabel()));
    AppendProperty(propGrid, selectedElement, new wxBoolProperty("Active", "Active", selectedElement.isActive()));
    AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Transform"));
    auto transform = selectedElement.getTransform();
    AppendProperty(propGrid, selectedElement, new wxFloatProperty("Position X", "PosX", transform.position.x));
    AppendProperty(propGrid, selectedElement, new wxFloatProperty("Position Y", "PosY", transform.position.y));
    AppendProperty(propGrid, selectedElement, new wxFloatProperty("Position Z", "zValue", transform.zValue));
    AppendProperty(propGrid, selectedElement, new wxFloatProperty("Scale X", "ScaleX", transform.scale.x));
    AppendProperty(propGrid, selectedElement, new wxFloatProperty("Scale Y", "ScaleY", transform.scale.y));

    AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Screen scale"));
    AppendProperty(propGrid, selectedElement,
                   new wxFloatProperty("Screen position X", "SPosX", selectedElement.getScreenPosition().x));
    AppendProperty(propGrid, selectedElement,
                   new wxFloatProperty("Screen position Y", "SPosY", selectedElement.getScreenPosition().y));
    AppendProperty(propGrid, selectedElement, new wxFloatProperty("Screen position Z", "SPosZ", selectedElement.getZValue()));
    AppendProperty(propGrid, selectedElement,
                   new wxFloatProperty("Screen scale X", "SScaleX", selectedElement.getScreenScale().x));
    AppendProperty(propGrid, selectedElement,
                   new wxFloatProperty("Screen scale Y", "SScaleY", selectedElement.getScreenScale().y));
};

void GuiEditorFrame::renderAbleProperties(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement)
{
    if (auto* el = dynamic_cast<GameEngine::GUI::RenderAble*>(&selectedElement))
    {
        AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Renderer Base Settings"));
        AppendColorProperty(propGrid, el->getColor(), "Color", "ElementColor");
        AppendProperty(propGrid, selectedElement,
                       new wxIntProperty("Inactivity release", "InactivityRelease", el->inactivityRelease()));
    }
}

void GuiEditorFrame::textProperties(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement)
{
    if (auto* txt = dynamic_cast<GameEngine::GUI::Text*>(&selectedElement))
    {
        auto renderModes = CreateChoicesFromEnum<GameEngine::GUI::RenderMode>();
        auto renderMode  = magic_enum::enum_index(txt->render.mode.get());

        auto alignes = CreateChoicesFromEnum<GameEngine::GUI::HorizontalAlign>();
        auto align   = magic_enum::enum_index(txt->render.align.get());

        AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Text Settings"));
        AppendProperty(propGrid, selectedElement, new wxStringProperty("Label text", "LabelText", txt->getText()));
        AppendProperty(propGrid, selectedElement,
                       new wxIntProperty("Wrap width", "TextWrapWith", txt->text.wrapWidth.get().value_or(0)));
        AppendProperty(propGrid, selectedElement,
                       new wxEnumProperty("Render mode", "TextRenderMode", renderModes, renderMode.value_or(0)));
        AppendProperty(propGrid, selectedElement, new wxEnumProperty("HorizontalAlign", "TextAlign", alignes, align.value_or(0)));
        AppendProperty(propGrid, selectedElement, new wxIntProperty("Outline", "TextOutline", txt->font.outline));

        AppendProperty(propGrid, selectedElement,
                       new wxBoolProperty("Auto font size", "TextAutoFontSize", not txt->font.size->has_value()));
        if (txt->font.size->has_value())
        {
            AppendProperty(propGrid, selectedElement,
                           new wxIntProperty("Font size", "TextFontSize", txt->font.size.get().value()));
        }

        std::string filepath;
        const auto& maybeFileHanle = txt->font.file.get();
        if (maybeFileHanle.has_value())
        {
            filepath = maybeFileHanle->getPath();
        }

        AppendProperty(propGrid, selectedElement, new FileHandleProperty("Font", "TextFontFile", filepath));
    }
}

void GuiEditorFrame::multiLineTextProperties(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement)
{
    if (auto* txt = dynamic_cast<GameEngine::GUI::MultiLineText*>(&selectedElement))
    {
        auto renderModes = CreateChoicesFromEnum<GameEngine::GUI::RenderMode>();
        auto renderMode  = magic_enum::enum_index(txt->render.mode.get());

        auto alignes = CreateChoicesFromEnum<GameEngine::GUI::HorizontalAlign>();
        auto align   = magic_enum::enum_index(txt->render.align.get());

        AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Text Settings"));
        AppendProperty(propGrid, selectedElement, new wxStringProperty("Label text", "LabelText", txt->getText()));
        AppendProperty(propGrid, selectedElement,
                       new wxFloatProperty("Local Line height", "MTextLineHeight", txt->lineHeight.get()));
        AppendProperty(propGrid, selectedElement,
                       new wxFloatProperty("Screen Line height", "MTextSLineHeight", txt->getScreenScaleLineHeight()));

        AppendProperty(propGrid, selectedElement,
                       new wxIntProperty("Wrap width", "TextWrapWith", txt->text.wrapWidth.get().value_or(0)));
        AppendProperty(propGrid, selectedElement,
                       new wxEnumProperty("Render mode", "TextRenderMode", renderModes, renderMode.value_or(0)));
        AppendProperty(propGrid, selectedElement, new wxEnumProperty("HorizontalAlign", "TextAlign", alignes, align.value_or(0)));
        AppendProperty(propGrid, selectedElement, new wxIntProperty("Outline", "TextOutline", txt->font.outline));

        AppendProperty(propGrid, selectedElement,
                       new wxBoolProperty("Auto font size", "TextAutoFontSize", not txt->font.size->has_value()));
        if (txt->font.size->has_value())
        {
            AppendProperty(propGrid, selectedElement,
                           new wxIntProperty("Font size", "TextFontSize", txt->font.size.get().value()));
        }

        std::string filepath;
        const auto& maybeFileHanle = txt->font.file.get();
        if (maybeFileHanle.has_value())
        {
            filepath = maybeFileHanle->getPath();
        }

        AppendProperty(propGrid, selectedElement, new FileHandleProperty("Font", "TextFontFile", filepath));
    }
}

void GuiEditorFrame::spriteProperties(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement)
{
    if (auto* txt = dynamic_cast<GameEngine::GUI::Sprite*>(&selectedElement))
    {
        AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Texture Settings"));

        auto texture = txt->getTexture();
        std::string filePath{};
        if (texture and texture->GetFile())
        {
            filePath = texture->GetFile()->getPath();
        }
        AppendProperty(propGrid, selectedElement, new wxFileProperty("Image path", "ImagePath", filePath));
    }
}

void GuiEditorFrame::buttonProperties(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement)
{
    if (auto* button = dynamic_cast<GameEngine::GUI::Button*>(&selectedElement))
    {
        AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Button Settings"));

        AppendProperty(propGrid, selectedElement, new wxStringProperty("Label text", "ButtonLabelText", button->getText()));
        AppendProperty(propGrid, selectedElement,
                       new wxFloatProperty("Text scale", "ButtonLabelTextScale", button->getTextScale()));
        AppendColorProperty(propGrid, button->getBackgroundColor(), "Normal Color", "ButtonBackgorundColor");
        AppendColorProperty(propGrid, button->getOnHoverColor(), "On Hover Color", "ButtonHoverColor");
        AppendColorProperty(propGrid, button->getActiveColor(), "Active Color", "ButtonActiveColor");

        if (auto text = button->getTextElement())
        {
            textProperties(propGrid, *text);
        }
    }
}

void GuiEditorFrame::layoutProperties(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement)
{
    if (auto* txt = dynamic_cast<GameEngine::GUI::Layout*>(&selectedElement))
    {
        AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Layout"));

        auto hAlignes = CreateChoicesFromEnum<GameEngine::GUI::HorizontalAlign>();
        auto hAlign   = magic_enum::enum_index(txt->getHorizontalAlign());

        auto vAlignes = CreateChoicesFromEnum<GameEngine::GUI::VerticalAlign>();
        auto vAlign   = magic_enum::enum_index(txt->getVerticalAlign());

        AppendProperty(propGrid, selectedElement,
                       new wxEnumProperty("Horizontal align", "LayoutHorizontalAlign", hAlignes, hAlign.value_or(0)));
        AppendProperty(propGrid, selectedElement,
                       new wxEnumProperty("Vertical align", "LayoutVerticalAlign", vAlignes, vAlign.value_or(0)));
    }
}

void GuiEditorFrame::verticalProperties(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement)
{
    if (auto* txt = dynamic_cast<GameEngine::GUI::VerticalLayout*>(&selectedElement))
    {
        AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Vertical Layout"));
        AppendProperty(propGrid, selectedElement,
                       new wxBoolProperty("Auto hide elements", "VL_AutoHideElements", txt->autoHideElements()));
    }
}

void GuiEditorFrame::windowProperties(wxPropertyGrid& propGrid, GameEngine::GUI::Element& selectedElement)
{
    if (auto* window = dynamic_cast<GameEngine::GUI::Window*>(&selectedElement))
    {
        auto choices = CreateChoicesFromEnum<GameEngine::GUI::WindowStyle>();
        auto index   = magic_enum::enum_index(window->getStyle());

        AppendProperty(propGrid, selectedElement, new wxPropertyCategory("Window Settings"));
        AppendProperty(propGrid, selectedElement, new wxEnumProperty("Style", "GUI::WindowStyle", choices, index.value_or(0)));
    }
}

void GuiEditorFrame::OnTreeSelectionChanged(wxTreeEvent& event)
{
    if (not propGrid)
    {
        return;
    }

    propGrid->Clear();
    auto id = event.GetItem();
    if (not id.IsOk())
        return;

    auto* data      = static_cast<GuiTreeItemData*>(sceneTree->GetItemData(id));
    selectedElement = data ? data->element : nullptr;

    rebuildPropertiesView();
}

void GuiEditorFrame::OnPropertyChange(wxPropertyGridEvent& event)
{
    LOG_DEBUG << "";
    auto* p   = event.GetProperty();
    auto name = p->GetName();

    auto* target = static_cast<GameEngine::GUI::Element*>(p->GetClientData());
    if (target)
    {
        if (name == "GUI::WindowStyle")
        {
            if (auto window = dynamic_cast<GameEngine::GUI::Window*>(selectedElement))
            {
                auto intVal = p->GetValue().GetInteger();
                if (auto style = magic_enum::enum_cast<GameEngine::GUI::WindowStyle>(intVal))
                {
                    window->setStyle(*style);
                }
            }
        }
        else if (name == "Active")
        {
            target->activate(p->GetValue().GetBool());
        }
        else if (name == "ElementLabelText")
        {
            auto str = p->GetValue().GetString().ToStdString();
            target->setLabel(str);
        }
        else if (name == "PosX")
        {
            auto pos = target->getLocalPosition();
            pos.x    = p->GetValue().GetDouble();
            target->setLocalPosition(pos);
        }
        else if (name == "PosY")
        {
            auto pos = target->getLocalPosition();
            pos.y    = p->GetValue().GetDouble();
            target->setLocalPosition(pos);
        }
        else if (name == "zValue")
        {
            auto z = p->GetValue().GetDouble();
            target->setZPosition(z);
        }
        else if (name == "ScaleX")
        {
            auto scale = target->getLocalScale();
            scale.x    = p->GetValue().GetDouble();
            target->setLocalScale(scale);
        }
        else if (name == "ScaleY")
        {
            auto scale = target->getLocalScale();
            scale.y    = p->GetValue().GetDouble();
            target->setLocalScale(scale);
        }
        else if (name == "SPosX")
        {
            auto pos = target->getScreenPosition();
            pos.x    = p->GetValue().GetDouble();
            target->setScreenPostion(pos);
        }
        else if (name == "SPosY")
        {
            auto pos = target->getScreenPosition();
            pos.y    = p->GetValue().GetDouble();
            target->setScreenPostion(pos);
        }
        // else if (name == "SzValue")
        // {
        //     auto z = p->GetValue().GetDouble();
        //     target->setZPosition(z);
        // }
        else if (name == "SScaleX")
        {
            auto scale = target->getScreenScale();
            scale.x    = p->GetValue().GetDouble();
            target->setScreenScale(scale);
        }
        else if (name == "SScaleY")
        {
            auto scale = target->getScreenScale();
            scale.y    = p->GetValue().GetDouble();
            target->setScreenScale(scale);
        }
        else if (name == "ElementColor")
        {
            if (auto el = dynamic_cast<GameEngine::GUI::RenderAble*>(target))
            {
                el->setColor(ConvertVariantToColor(p->GetValue()));
            }
        }
        else if (name == "InactivityRelease")
        {
            if (auto el = dynamic_cast<GameEngine::GUI::RenderAble*>(target))
            {
                el->inactivityRelease(p->GetValue().GetInteger());
            }
        }
        else if (name == "ButtonBackgorundColor")
        {
            if (auto el = dynamic_cast<GameEngine::GUI::Button*>(target))
            {
                el->setBackground(ConvertVariantToColor(p->GetValue()));
            }
        }
        else if (name == "ButtonHoverColor")
        {
            if (auto el = dynamic_cast<GameEngine::GUI::Button*>(target))
            {
                el->setOnHover(ConvertVariantToColor(p->GetValue()));
            }
        }
        else if (name == "ButtonActiveColor")
        {
            if (auto el = dynamic_cast<GameEngine::GUI::Button*>(target))
            {
                el->setOnActive(ConvertVariantToColor(p->GetValue()));
            }
        }
        else if (name == "ButtonLabelText")
        {
            if (auto el = dynamic_cast<GameEngine::GUI::Button*>(target))
            {
                auto str = p->GetValue().GetString().ToStdString();
                if (not str.empty())
                {
                    if (auto text = el->getTextElement())
                    {
                        text->setText(str);
                    }
                    else
                    {
                        auto& factory = canvas->GetScene().GetGuiElementFactory();
                        auto newElem  = factory.createText(str);
                        el->setText(std::move(newElem));
                    }
                }
                else
                {
                    el->setText(nullptr);
                }
            }
        }
        else if (name == "ButtonLabelTextScale")
        {
            if (auto el = dynamic_cast<GameEngine::GUI::Button*>(target))
            {
                el->setTextScale(p->GetValue().GetDouble());
            }
        }
        else if (name == "LabelText")
        {
            auto str = p->GetValue().GetString().ToStdString();
            if (auto text = dynamic_cast<GameEngine::GUI::Text*>(target))
            {
                text->setText(str);
                RefreshTree();
            }
            else if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(target))
            {
                LOG_DEBUG << "set " << str;
                text->setText(str);
                RefreshTree();
            }
        }
        else if (name == "MTextLineHeight")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(target))
            {
                text->lineHeight = p->GetValue().GetDouble();
            }
        }
        else if (name == "MTextSLineHeight")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(target))
            {
                text->setScreenScaleLineHeight(p->GetValue().GetDouble());
            }
        }
        else if (name == "TextWrapWith")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::Text*>(target))
            {
                text->text.wrapWidth = p->GetValue().GetInteger();
            }
            else if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(target))
            {
                text->text.wrapWidth = p->GetValue().GetInteger();
            }
        }
        else if (name == "TextRenderMode")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::Text*>(target))
            {
                int intVal = p->GetValue().GetInteger();
                if (auto renderMode = magic_enum::enum_cast<GameEngine::GUI::RenderMode>(intVal))
                {
                    text->render.mode = *renderMode;
                }
            }
        }
        else if (name == "TextAlign")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::Text*>(target))
            {
                auto intVal = p->GetValue().GetInteger();
                if (auto v = magic_enum::enum_cast<GameEngine::GUI::HorizontalAlign>(intVal))
                {
                    text->render.align = *v;
                }
            }
            else if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(target))
            {
                auto intVal = p->GetValue().GetInteger();
                if (auto v = magic_enum::enum_cast<GameEngine::GUI::HorizontalAlign>(intVal))
                {
                    text->render.align = *v;
                }
            }
        }
        else if (name == "TextOutline")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::Text*>(target))
            {
                text->font.outline = p->GetValue().GetInteger();
            }
            else if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(target))
            {
                text->font.outline = p->GetValue().GetInteger();
            }
        }
        else if (name == "TextAutoFontSize")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::Text*>(target))
            {
                if (p->GetValue().GetBool())
                {
                    text->font.size = 64;
                }
                else
                {
                    text->font.size = std::nullopt;
                }

                rebuildPropertiesView();
            }
            else if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(target))
            {
                if (p->GetValue().GetBool())
                {
                    text->font.size = 64;
                }
                else
                {
                    text->font.size = std::nullopt;
                }

                rebuildPropertiesView();
            }
        }
        else if (name == "TextFontSize")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::Text*>(target))
            {
                text->font.size = p->GetValue().GetInteger();
            }
            else if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(target))
            {
                text->font.size = p->GetValue().GetInteger();
            }
        }
        else if (name == "TextFontFile")
        {
            LOG_DEBUG << p->GetValue().GetString().ToStdString();
            if (auto text = dynamic_cast<GameEngine::GUI::Text*>(target))
            {
                LOG_DEBUG << p->GetValue().GetString().ToStdString();
                text->font.file = p->GetValue().GetString().ToStdString();
            }
            else if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(target))
            {
                LOG_DEBUG << p->GetValue().GetString().ToStdString();
                text->font.file = p->GetValue().GetString().ToStdString();
            }
            else
            {
                LOG_DEBUG << p->GetValue().GetString().ToStdString();
            }
        }
        else if (name == "ImagePath")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::Sprite*>(target))
            {
                text->SetTexture(p->GetValue().GetString().ToStdString());
            }
        }
        else if (name == "VL_AutoHideElements")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::VerticalLayout*>(target))
            {
                text->autoHideElements(p->GetValue().GetBool());
            }
        }
        else if (name == "LayoutHorizontalAlign")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::Layout*>(target))
            {
                auto intVal = p->GetValue().GetInteger();
                if (auto v = magic_enum::enum_cast<GameEngine::GUI::HorizontalAlign>(intVal))
                {
                    text->setAlign(*v);
                }
            }
        }
        else if (name == "LayoutVerticalAlign")
        {
            if (auto text = dynamic_cast<GameEngine::GUI::Layout*>(target))
            {
                auto intVal = p->GetValue().GetInteger();
                if (auto v = magic_enum::enum_cast<GameEngine::GUI::VerticalAlign>(intVal))
                {
                    text->setAlign(*v);
                }
            }
        }
    }

    rebuildPropertiesView();
    canvas->Refresh();
}

void GuiEditorFrame::OnOpen(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, "Choose file", lastDirPath.string(), "", "Gui file (*.gui)|*.gui|All files (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    auto path = openFileDialog.GetPath();
    if (OpenFile(path.ToStdString()))
    {
        AddToRecentFile(path.ToStdString());
    }
}
void GuiEditorFrame::OnReOpen(wxCommandEvent&)
{
    if (currentFile)
    {
        OpenFile(*currentFile);
    }
}
void GuiEditorFrame::OnOpenRecent(wxCommandEvent& event)
{
    int index = event.GetId() - ID_RECENT_FIRST;

    if (index >= 0 && index < (int)recentFiles.size())
    {
        std::filesystem::path path = recentFiles[index];

        if (std::filesystem::exists(path))
        {
            OpenFile(path);
        }
        else
        {
            wxMessageBox("File no longer exists!", "Error", wxICON_ERROR);
            recentFiles.erase(recentFiles.begin() + index);
            UpdateRecentFilesMenu();
            SaveRecentFiles();
        }
    }
}
void GuiEditorFrame::OnSave(wxCommandEvent& e)
{
    if (currentFile)
    {
        if (auto layer = canvas->GetScene().GetGuiManager().getLayer(GameEngine::GUI::DEFAULT_LAYER))
        {
            GameEngine::GUI::ElementWriter::write(currentFile->GetAbsolutePath(), *layer);
        }
    }
    else
    {
        OnSaveAs(e);
    }
}
void GuiEditorFrame::OnSaveAs(wxCommandEvent&)
{
    wxFileDialog fileDialog(this, "Choose file", lastDirPath.string(), "", "Gui file (*.gui)|*.gui|All files (*.*)|*.*",
                            wxFD_SAVE);

    if (fileDialog.ShowModal() == wxID_CANCEL)
        return;

    auto path = fileDialog.GetPath();
    GameEngine::File file{std::string{path.c_str()}};

    if (not file.HasExtension())
    {
        file.ChangeExtension("gui");
    }

    if (auto layer = canvas->GetScene().GetGuiManager().getLayer(GameEngine::GUI::DEFAULT_LAYER))
    {
        GameEngine::GUI::ElementWriter::write(file, *layer);
    }

    currentFile = file;
    lastDirPath = file.GetAbsolutePath().parent_path();
    AddToRecentFile(file);
}
void GuiEditorFrame::OnUndo(wxCommandEvent&)
{
    /* Logika Undo w silniku */
}
void GuiEditorFrame::OnRedo(wxCommandEvent&)
{
    /* Logika Redo w silniku */
}
void GuiEditorFrame::OnExit(wxCommandEvent&)
{
    Close(true);
}

void GuiEditorFrame::OnTimer(wxTimerEvent&)
{
    if (canvas and canvas->IsShownOnScreen())
        canvas->Refresh();
}

void GuiEditorFrame::RefreshTree()
{
    sceneTree->DeleteAllItems();
    auto root  = sceneTree->AddRoot("GUI Root");
    auto layer = canvas->GetScene().GetGuiManager().getLayer(GameEngine::GUI::DEFAULT_LAYER);
    for (auto& element : layer->get())
    {
        AddTreeItem(root, element.get());
    }

    sceneTree->Expand(root);
}

void GuiEditorFrame::AddTreeItem(wxTreeItemId parentId, GameEngine::GUI::Element* element)
{
    if (not element)
        return;

    std::string itemText = "Element";

    if (auto window = dynamic_cast<GameEngine::GUI::Window*>(element))
    {
        itemText = "[Window]  ";
    }
    else if (auto button = dynamic_cast<GameEngine::GUI::Button*>(element))
    {
        itemText = "[Button] ";
    }
    else if (auto text = dynamic_cast<GameEngine::GUI::Text*>(element))
    {
        std::string content = text->getText();
        if (content.length() > 15)
            content = content.substr(0, 12) + "...";
        itemText = "[Text] ";
    }
    else if (auto text = dynamic_cast<GameEngine::GUI::MultiLineText*>(element))
    {
        std::string content = text->getText();
        if (content.length() > 15)
            content = content.substr(0, 12) + "...";
        itemText = "[MultiLineText] ";
    }
    else if (auto tex = dynamic_cast<GameEngine::GUI::Sprite*>(element))
    {
        itemText = "[Image]  ";
    }
    else if (auto edit = dynamic_cast<GameEngine::GUI::EditText*>(element))
    {
        itemText = "[EditBox] ";
    }
    else if (dynamic_cast<GameEngine::GUI::VerticalLayout*>(element))
    {
        itemText = "[V-Layout] (" + std::to_string(element->getChildren().size()) + ")  ";
    }
    else if (dynamic_cast<GameEngine::GUI::HorizontalLayout*>(element))
    {
        itemText = "[H-Layout] (" + std::to_string(element->getChildren().size()) + ")  ";
    }

    auto item = sceneTree->AppendItem(parentId, itemText + element->getLabel(), -1, -1, new GuiTreeItemData(element));
    for (const auto& child : element->getChildren())
    {
        AddTreeItem(item, child.get());
    }
}
void GuiEditorFrame::FocusElementInTree(GameEngine::GUI::Element* target)
{
    if (not target)
        return;

    auto root = sceneTree->GetRootItem();
    if (not root.IsOk())
        return;

    auto found = FindItemByElement(root, target);
    if (found.IsOk())
    {
        sceneTree->SelectItem(found);
        sceneTree->EnsureVisible(found);
    }
}
wxTreeItemId GuiEditorFrame::FindItemByElement(wxTreeItemId parent, GameEngine::GUI::Element* target)
{
    wxTreeItemIdValue cookie;
    auto child = sceneTree->GetFirstChild(parent, cookie);

    while (child.IsOk())
    {
        auto* data = static_cast<GuiTreeItemData*>(sceneTree->GetItemData(child));

        if (data and data->element == target)
        {
            return child;
        }

        if (sceneTree->ItemHasChildren(child))
        {
            auto found = FindItemByElement(child, target);
            if (found.IsOk())
                return found;
        }

        child = sceneTree->GetNextChild(parent, cookie);
    }

    return wxTreeItemId();
}
void GuiEditorFrame::OnTreeContextMenu(wxTreeEvent& event)
{
    auto itemId = event.GetItem();
    if (not itemId.IsOk())
        return;

    auto* data = static_cast<GuiTreeItemData*>(sceneTree->GetItemData(itemId));
    // if (not data)
    //     return;

    int idWin         = wxWindow::NewControlId();
    int idText        = wxWindow::NewControlId();
    int idMultiText   = wxWindow::NewControlId();
    int idClone       = wxWindow::NewControlId();
    int idBtn         = wxWindow::NewControlId();
    int idTex         = wxWindow::NewControlId();
    int idVLayout     = wxWindow::NewControlId();
    int idHLayout     = wxWindow::NewControlId();
    int idDel         = wxWindow::NewControlId();
    int idDelChildren = wxWindow::NewControlId();

    wxMenu menu;
    wxMenu* addSubMenu = new wxMenu();
    addSubMenu->Append(idWin, "Window");
    addSubMenu->Append(idText, "Text Label");
    addSubMenu->Append(idMultiText, "Multi line text");
    addSubMenu->Append(idTex, "Image/Texture");
    addSubMenu->Append(idBtn, "Button");
    addSubMenu->AppendSeparator();
    addSubMenu->Append(idVLayout, "Vertical Layout");
    addSubMenu->Append(idHLayout, "Horizontal Layout");

    menu.AppendSubMenu(addSubMenu, "Add Child");
    menu.Append(idClone, "Clone");

    if (data)
    {
        menu.AppendSeparator();
        menu.Append(idDel, "Delete");
    }

    menu.Append(idDelChildren, "Delete all children");

    int selection = GetPopupMenuSelectionFromUser(menu);

    if (selection == wxID_NONE)
        return;

    if (data and selection == idClone)
    {
        if (data->element)
        {
            auto element      = data->element->clone();
            auto* ptrToSelect = element.get();
            if (auto parent = data->element->getParent())
            {
                parent->addChild(std::move(element));
            }
            else
            {
                canvas->GetScene().GetGuiManager().add(std::move(element));
            }

            RefreshTree();
            FocusElementInTree(ptrToSelect);
            canvas->Refresh();
        }
        return;
    }

    if (data and selection == idDel)
    {
        OnDeleteElement(*data);
        return;
    }

    if (data and selection == idDelChildren)
    {
        data->element->removeAll();
        RefreshTree();
        FocusElementInTree(data->element);
        canvas->Refresh();
        return;
    }

    auto& factory = canvas->GetScene().GetGuiElementFactory();
    std::unique_ptr<GameEngine::GUI::Element> newElem;

    if (selection == idWin)
        newElem = factory.createWindow(GameEngine::GUI::WindowStyle::BACKGROUND_ONLY);
    else if (selection == idText)
        newElem = factory.createText("New Label");
    else if (selection == idMultiText)
        newElem = factory.createMultiLineText(LONG_DUMMY_TEXT);
    else if (selection == idTex)
        newElem = factory.createSprite("DefaultTexture.png");
    else if (selection == idBtn)
        newElem = factory.createButton();
    else if (selection == idVLayout)
        newElem = factory.createVerticalLayout();
    else if (selection == idHLayout)
        newElem = factory.createHorizontalLayout();

    if (newElem)
    {
        auto* ptrToSelect = newElem.get();
        if (data and data->element)
        {
            data->element->addChild(std::move(newElem));
        }
        else
        {
            canvas->GetScene().GetGuiManager().add(std::move(newElem));
        }

        RefreshTree();
        FocusElementInTree(ptrToSelect);
        canvas->Refresh();
    }
}
void GuiEditorFrame::OnDeleteElement(GuiTreeItemData& data)
{
    if (data.element)
    {
        if (canvas->GetScene().GetGuiManager().remove(*data.element))
        {
            selectedElement = nullptr;
            propGrid->Clear();
            RefreshTree();
            canvas->Refresh();
        }
        else
        {
            wxLogError("Remove error");
        }
    }
}
void GuiEditorFrame::ReadRecentOpenedFiles()
{
    recentFiles.clear();

    wxConfig config("GuiEditor");

    long count = 0;
    config.Read("Recent/Count", &count, 0);

    for (long i = 0; i < count; i++)
    {
        wxString key    = wxString::Format("Recent/File%ld", i);
        wxString wxPath = config.Read(key, "");

        if (not wxPath.IsEmpty())
        {
            std::filesystem::path p(wxPath.ToStdWstring());
            p.make_preferred();

            if (std::filesystem::exists(p))
            {
                recentFiles.push_back(p);
            }
        }
    }
}
void GuiEditorFrame::SaveRecentFiles()
{
    wxConfig config("GuiEditor");
    config.DeleteGroup("Recent");
    config.Write("Recent/Count", static_cast<long>(recentFiles.size()));
    for (size_t i = 0; i < recentFiles.size(); ++i)
    {
        wxString key = wxString::Format("Recent/File%zu", i);
        wxString wxPath(recentFiles[i].wstring());
        config.Write(key, wxPath);
    }
    config.Flush();
}
void GuiEditorFrame::AddToRecentFile(const GameEngine::File& file)
{
    auto normalizedPath = file.GetAbsolutePath().lexically_normal();
    normalizedPath.make_preferred();
    recentFiles.erase(std::remove(recentFiles.begin(), recentFiles.end(), normalizedPath), recentFiles.end());
    recentFiles.insert(recentFiles.begin(), normalizedPath);
    if (recentFiles.size() > 10)
    {
        recentFiles.resize(10);
    }

    SaveRecentFiles();
    UpdateRecentFilesMenu();
}
void GuiEditorFrame::UpdateRecentFilesMenu()
{
    if (!recentMenu)
        return;

    for (size_t i = recentMenu->GetMenuItemCount(); i > 0; i--)
    {
        recentMenu->Destroy(recentMenu->FindItemByPosition(i - 1));
    }

    if (recentFiles.empty())
    {
        auto* item = recentMenu->Append(wxID_NONE, "No Recent Files");
        item->Enable(false);
        return;
    }

    for (size_t i = 0; i < recentFiles.size() && i < 10; ++i)
    {
        auto label = wxString::Format("&%zu %s", i + 1, recentFiles[i].filename().string());
        recentMenu->Append(ID_RECENT_FIRST + i, label, recentFiles[i].string());
    }
}
bool GuiEditorFrame::OpenFile(const GameEngine::File& file)
{
    if (not file.exist())
        return false;

    propGrid->Clear();
    canvas->GetScene().GetGuiManager().removeAllFromLayer(GameEngine::GUI::DEFAULT_LAYER);
    GameEngine::GUI::ElementReader reader(canvas->GetScene().GetGuiManager(), canvas->GetScene().GetGuiElementFactory());
    if (reader.read(file))
    {
        currentFile = file;
        lastDirPath = currentFile->GetAbsolutePath().parent_path();
        RefreshTree();
        return true;
    }

    return false;
}
void GuiEditorFrame::rebuildPropertiesView()
{
    propGrid->Clear();
    if (selectedElement)
    {
        LOG_DEBUG << "OnTreeSelectionChanged add properies";
        commonProperties(*propGrid, *selectedElement);
        renderAbleProperties(*propGrid, *selectedElement);
        windowProperties(*propGrid, *selectedElement);
        buttonProperties(*propGrid, *selectedElement);
        textProperties(*propGrid, *selectedElement);
        multiLineTextProperties(*propGrid, *selectedElement);
        spriteProperties(*propGrid, *selectedElement);
        layoutProperties(*propGrid, *selectedElement);
        verticalProperties(*propGrid, *selectedElement);
    }
}
