#include "GuiEditorFrame.h"

#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/GuiElement.h>
#include <GameEngine/Renderers/GUI/GuiElementReader.h>
#include <GameEngine/Renderers/GUI/GuiElementsDef.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/GUI/TreeView/TreeView.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <wx/propgrid/advprops.h>

#include <unordered_map>
#include <vector>

#include "AddElementDialog.h"
#include "GuiEditorControlIds.h"
#include "GuiTreeItemData.h"
#include "Renderers/GUI/EditText/GuiEditText.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "Renderers/GUI/Window/GuiWindowStyle.h"
#include "Types.h"
#include "WxEditor/EngineRelated/GLCanvas.h"
#include "WxEditor/EngineRelated/WxScenesDef.h"
#include "WxEditor/ProjectManager.h"
#include "magic_enum/magic_enum.hpp"
#include "wx/log.h"
#include "wx/propgrid/props.h"

namespace
{
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
void AppendColorProperty(wxPropertyGrid* pg, const Color& color, const wxString& label = "Color",
                         const wxString& propName = "ElementColor")
{
    if (not pg)
        return;

    wxColour wxC(color.r(), color.g(), color.b(), color.a());
    wxPGProperty* p = pg->Append(new wxColourProperty(label, propName, wxC));
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
    std::unique_ptr<GameEngine::GuiElement> newElem;

    switch (event.GetId())
    {
        case ID_ADD_WINDOW:
            newElem = factory.CreateGuiWindow(GameEngine::GuiWindowStyle::BACKGROUND_ONLY, {0.5, 0.5}, {0.8, 0.6});
            break;
        case ID_ADD_TEXT:
            newElem = factory.CreateGuiText("New Label");
            break;
        case ID_ADD_TEXTURE:
            newElem = factory.CreateGuiTexture("DefaultTexture.png");
            break;
        case ID_ADD_BUTTON:
            newElem = factory.CreateGuiButton("Button", [](GameEngine::GuiElement&) {});
            break;
        case ID_ADD_V_LAYOUT:
            newElem = factory.CreateVerticalLayout();
            break;
        case ID_ADD_H_LAYOUT:
            newElem = factory.CreateHorizontalLayout();
            break;
    }

    if (newElem)
    {
        auto* ptrToSelect = newElem.get();
        canvas->GetScene().GetGuiManager().Add(std::move(newElem));
        RefreshTree();
        FocusElementInTree(ptrToSelect);
        canvas->Refresh();
    }
}

void GuiEditorFrame::OnTreeSelectionChanged(wxTreeEvent& event)
{
    propGrid->Clear();
    auto id = event.GetItem();
    if (not id.IsOk())
        return;

    auto* data      = static_cast<GuiTreeItemData*>(sceneTree->GetItemData(id));
    selectedElement = data ? data->element : nullptr;

    if (selectedElement and data->element)
    {
        propGrid->Append(new wxPropertyCategory("Transform"));
        auto pos   = selectedElement->GetLocalPosition();
        auto scale = selectedElement->GetLocalScale();
        propGrid->Append(new wxFloatProperty("Position X", "PosX", pos.x));
        propGrid->Append(new wxFloatProperty("Position Y", "PosY", pos.y));
        propGrid->Append(new wxFloatProperty("Scale X", "ScaleX", scale.x));
        propGrid->Append(new wxFloatProperty("Scale Y", "ScaleY", scale.y));

        if (auto* el = dynamic_cast<GameEngine::GuiRendererElementBase*>(selectedElement))
        {
            propGrid->Append(new wxPropertyCategory("Renderer Base Settings"));
            AppendColorProperty(propGrid, el->GetColor(), "Color", "ElementColor");
        }

        if (auto* window = dynamic_cast<GameEngine::GuiWindowElement*>(selectedElement))
        {
            auto choices = CreateChoicesFromEnum<GameEngine::GuiWindowStyle>();
            auto index   = magic_enum::enum_index(window->GetStyle());

            propGrid->Append(new wxPropertyCategory("Window Settings"));
            propGrid->Append(new wxEnumProperty("Style", "GuiWindowStyle", choices, index.value_or(0)));
        }
        else if (auto* txt = dynamic_cast<GameEngine::GuiButtonElement*>(selectedElement))
        {
            propGrid->Append(new wxPropertyCategory("Button Settings"));
            // propGrid->Append(new wxStringProperty("Action name", "ActionName", ""));
            AppendColorProperty(propGrid, txt->GetBackgroundColor(), "Normal Color", "ButtonBackgorundColor");
            AppendColorProperty(propGrid, txt->GetOnHoverColor(), "On Hover Color", "ButtonHoverColor");
            AppendColorProperty(propGrid, txt->GetActiveColor(), "Active Color", "ButtonActiveColor");
        }
        else if (auto* txt = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
        {
            auto renderModes = CreateChoicesFromEnum<GameEngine::GuiTextElement::RenderMode>();
            auto renderMode  = magic_enum::enum_index(txt->GetRenderMode());

            auto algines = CreateChoicesFromEnum<GameEngine::GuiTextElement::Algin>();
            auto algin   = magic_enum::enum_index(txt->GetRenderMode());

            propGrid->Append(new wxPropertyCategory("Text Settings"));
            propGrid->Append(new wxStringProperty("Label text", "LabelText", txt->GetText()));
            propGrid->Append(new wxIntProperty("Wrap width", "TextWrapWith", txt->GetWrapWith()));
            propGrid->Append(new wxEnumProperty("Render mode", "TextRenderMode", renderModes, renderMode.value_or(0)));
            propGrid->Append(new wxEnumProperty("Algin", "TextAlgin", algines, algin.value_or(0)));
            propGrid->Append(new wxIntProperty("Outline", "TextOutline", txt->GetOutline()));
            propGrid->Append(new wxIntProperty("Font size", "TextFontSize", txt->GetFontSize()));
            propGrid->Append(new wxFileProperty("Font size", "TextFontFile", txt->GetFontFile().GetDataRelativePath().string()));
        }
        else if (auto* txt = dynamic_cast<GameEngine::GuiTextureElement*>(selectedElement))
        {
            propGrid->Append(new wxPropertyCategory("Texture Settings"));

            auto texture = txt->GetTexture();
            std::string filePath{};
            if (texture and texture->GetFile())
            {
                filePath = texture->GetFile()->GetDataRelativePath();
            }
            propGrid->Append(new wxFileProperty("Image path", "ImagePath", filePath));
        }
        else if (auto* txt = dynamic_cast<GameEngine::VerticalLayout*>(selectedElement))
        {
            propGrid->Append(new wxPropertyCategory("Vertical Layout"));
            propGrid->Append(new wxBoolProperty("Auto hide elements", "VL_AutoHideElements", txt->AutoHideElements()));
        }
    }
}

void GuiEditorFrame::OnPropertyChange(wxPropertyGridEvent& event)
{
    auto* p   = event.GetProperty();
    auto name = p->GetName();

    if (selectedElement)
    {
        if (name == "GuiWindowStyle")
        {
            if (auto window = dynamic_cast<GameEngine::GuiWindowElement*>(selectedElement))
            {
                auto intVal = p->GetValue().GetInteger();
                if (auto style = magic_enum::enum_cast<GameEngine::GuiWindowStyle>(intVal))
                {
                    window->SetStyle(*style);
                }
            }
        }
        else if (name == "PosX")
        {
            auto pos = selectedElement->GetLocalPosition();
            pos.x    = p->GetValue().GetDouble();
            selectedElement->SetLocalPosition(pos);
        }
        else if (name == "PosY")
        {
            auto pos = selectedElement->GetLocalPosition();
            pos.y    = p->GetValue().GetDouble();
            selectedElement->SetLocalPosition(pos);
        }
        else if (name == "ScaleX")
        {
            auto scale = selectedElement->GetLocalScale();
            scale.x    = p->GetValue().GetDouble();
            selectedElement->SetLocalScale(scale);
        }
        else if (name == "ScaleY")
        {
            auto scale = selectedElement->GetLocalScale();
            scale.y    = p->GetValue().GetDouble();
            selectedElement->SetLocalScale(scale);
        }
        else if (name == "ElementColor")
        {
            if (auto el = dynamic_cast<GameEngine::GuiRendererElementBase*>(selectedElement))
            {
                el->SetColor(ConvertVariantToColor(p->GetValue()));
            }
        }
        else if (name == "ButtonBackgorundColor")
        {
            if (auto el = dynamic_cast<GameEngine::GuiButtonElement*>(selectedElement))
            {
                el->SetBackgroundTextColor(ConvertVariantToColor(p->GetValue()));
            }
        }
        else if (name == "ButtonHoverColor")
        {
            if (auto el = dynamic_cast<GameEngine::GuiButtonElement*>(selectedElement))
            {
                el->SetHoverTextColor(ConvertVariantToColor(p->GetValue()));
            }
        }
        else if (name == "ButtonActiveColor")
        {
            if (auto el = dynamic_cast<GameEngine::GuiButtonElement*>(selectedElement))
            {
                el->SetActiveTextColor(ConvertVariantToColor(p->GetValue()));
            }
        }
        else if (name == "LabelText")
        {
            if (auto text = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
            {
                text->SetText(p->GetValue().GetString().ToStdString());
            }
        }
        else if (name == "TextWrapWith")
        {
            if (auto text = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
            {
                text->SetWrapWidth(p->GetValue().GetInteger());
            }
        }
        else if (name == "TextRenderMode")
        {
            if (auto text = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
            {
                int intVal = p->GetValue().GetInteger();
                if (auto renderMode = magic_enum::enum_cast<GameEngine::GuiTextElement::RenderMode>(intVal))
                {
                    text->setRenderMode(*renderMode);
                }
            }
        }
        else if (name == "TextAlgin")
        {
            if (auto text = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
            {
                auto intVal = p->GetValue().GetInteger();
                if (auto v = magic_enum::enum_cast<GameEngine::GuiTextElement::Algin>(intVal))
                {
                    text->SetAlgin(*v);
                }
            }
        }
        else if (name == "TextOutline")
        {
            if (auto text = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
            {
                text->SetOutline(p->GetValue().GetInteger());
            }
        }
        else if (name == "TextFontSize")
        {
            if (auto text = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
            {
                text->SetFontSize(p->GetValue().GetInteger());
            }
        }
        else if (name == "TextFontFile")
        {
            if (auto text = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
            {
                text->SetFont(p->GetValue().GetString().ToStdString());
            }
        }
        else if (name == "ImagePath")
        {
            if (auto text = dynamic_cast<GameEngine::GuiTextureElement*>(selectedElement))
            {
                text->SetTexture(p->GetValue().GetString().ToStdString());
            }
        }
        else if (name == "VL_AutoHideElements")
        {
            if (auto text = dynamic_cast<GameEngine::VerticalLayout*>(selectedElement))
            {
                text->AutoHideElements(p->GetValue().GetBool());
            }
        }
    }

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
        canvas->GetScene().GetGuiManager().SaveToFile(currentFile->GetAbsolutePath());
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

    canvas->GetScene().GetGuiManager().SaveToFile(file);
    currentFile = file;
    lastDirPath = currentFile->GetAbsolutePath().parent_path();
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
    auto layer = canvas->GetScene().GetGuiManager().GetLayer(GameEngine::Gui::DEFAULT_LAYER);
    for (auto& element : layer->GetElements())
    {
        AddTreeItem(root, element.get());
    }

    sceneTree->Expand(root);
}

void GuiEditorFrame::AddTreeItem(wxTreeItemId parentId, GameEngine::GuiElement* element)
{
    if (not element)
        return;

    std::string itemText = "Element";

    if (auto window = dynamic_cast<GameEngine::GuiWindowElement*>(element))
    {
        itemText = "[Window]";
    }
    else if (auto button = dynamic_cast<GameEngine::GuiButtonElement*>(element))
    {
        itemText = "[Button] " + (button->GetText() ? button->GetText()->GetText() : "No Label");
    }
    else if (auto text = dynamic_cast<GameEngine::GuiTextElement*>(element))
    {
        std::string content = text->GetText();
        if (content.length() > 15)
            content = content.substr(0, 12) + "...";  // Skracanie długich tekstów
        itemText = "[Text] \"" + content + "\"";
    }
    else if (auto tex = dynamic_cast<GameEngine::GuiTextureElement*>(element))
    {
        itemText = "[Image]";
    }
    else if (auto edit = dynamic_cast<GameEngine::GuiEditBoxElement*>(element))
    {
        itemText = "[EditBox] ";  // + edit->GetPlaceholder();
    }
    else if (dynamic_cast<GameEngine::VerticalLayout*>(element))
    {
        itemText = "[V-Layout] (" + std::to_string(element->GetChildren().size()) + ")";
    }
    else if (dynamic_cast<GameEngine::HorizontalLayout*>(element))
    {
        itemText = "[H-Layout] (" + std::to_string(element->GetChildren().size()) + ")";
    }
    else if (dynamic_cast<GameEngine::TreeView*>(element))
    {
        itemText = "[TreeView]";
    }

    auto item = sceneTree->AppendItem(parentId, itemText, -1, -1, new GuiTreeItemData(element));
    for (const auto& child : element->GetChildren())
    {
        AddTreeItem(item, child.get());
    }
}
void GuiEditorFrame::FocusElementInTree(GameEngine::GuiElement* target)
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
wxTreeItemId GuiEditorFrame::FindItemByElement(wxTreeItemId parent, GameEngine::GuiElement* target)
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
    if (not data)
        return;

    int idWin     = wxWindow::NewControlId();
    int idText    = wxWindow::NewControlId();
    int idBtn     = wxWindow::NewControlId();
    int idTex     = wxWindow::NewControlId();
    int idVLayout = wxWindow::NewControlId();
    int idHLayout = wxWindow::NewControlId();
    int idDel     = wxWindow::NewControlId();

    wxMenu menu;
    wxMenu* addSubMenu = new wxMenu();
    addSubMenu->Append(idWin, "Window");
    addSubMenu->Append(idText, "Text Label");
    addSubMenu->Append(idTex, "Image/Texture");
    addSubMenu->Append(idBtn, "Button");
    addSubMenu->AppendSeparator();
    addSubMenu->Append(idVLayout, "Vertical Layout");
    addSubMenu->Append(idHLayout, "Horizontal Layout");

    menu.AppendSubMenu(addSubMenu, "Add Child");
    menu.AppendSeparator();
    menu.Append(idDel, "Delete");

    int selection = GetPopupMenuSelectionFromUser(menu);

    if (selection == wxID_NONE)
        return;

    if (selection == idDel)
    {
        OnDeleteElement(*data);
        return;
    }

    auto& factory = canvas->GetScene().GetGuiElementFactory();
    std::unique_ptr<GameEngine::GuiElement> newElem;

    if (selection == idWin)
        newElem = factory.CreateGuiWindow(GameEngine::GuiWindowStyle::BACKGROUND_ONLY, {0.5, 0.5}, {0.8, 0.6});
    else if (selection == idText)
        newElem = factory.CreateGuiText("New Label");
    else if (selection == idTex)
        newElem = factory.CreateGuiTexture("DefaultTexture.png");
    else if (selection == idBtn)
        newElem = factory.CreateGuiButton("Button", [](auto&) {});
    else if (selection == idVLayout)
        newElem = factory.CreateVerticalLayout();
    else if (selection == idHLayout)
        newElem = factory.CreateHorizontalLayout();

    if (newElem)
    {
        if (data->element)
        {
            auto* ptrToSelect = newElem.get();
            data->element->AddChild(std::move(newElem));
            RefreshTree();
            FocusElementInTree(ptrToSelect);
            canvas->Refresh();
        }
    }
}
void GuiEditorFrame::OnDeleteElement(GuiTreeItemData& data)
{
    if (data.element)
    {
        if (canvas->GetScene().GetGuiManager().Remove(*data.element))
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

    canvas->GetScene().GetGuiManager().RemoveAllFromLayer(GameEngine::Gui::DEFAULT_LAYER);
    GameEngine::GuiElementReader reader(canvas->GetScene().GetGuiManager(), canvas->GetScene().GetGuiElementFactory());
    if (reader.Read(file))
    {
        currentFile = file;
        lastDirPath = currentFile->GetAbsolutePath().parent_path();
        RefreshTree();
        return true;
    }

    return false;
}
