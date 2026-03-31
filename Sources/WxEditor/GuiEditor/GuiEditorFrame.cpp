#include "GuiEditorFrame.h"

#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/GuiElement.h>
#include <GameEngine/Renderers/GUI/GuiElementReader.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/GUI/TreeView/TreeView.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>

#include <unordered_map>

#include "AddElementDialog.h"
#include "GuiEditorControlIds.h"
#include "GuiTreeItemData.h"
#include "Renderers/GUI/EditText/GuiEditText.h"
#include "WxEditor/EngineRelated/GLCanvas.h"
#include "WxEditor/EngineRelated/WxScenesDef.h"
#include "WxEditor/ProjectManager.h"
#include "wx/propgrid/props.h"

GuiEditorFrame::GuiEditorFrame(const std::optional<GameEngine::File>& maybeFile, const wxString& title, const wxPoint& pos,
                               const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
    , currentFile(maybeFile)
{
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

    sceneTree->Bind(wxEVT_TREE_ITEM_MENU, &GuiEditorFrame::OnTreeContextMenu, this);
}

void GuiEditorFrame::OnAddElement(wxCommandEvent& event)
{
    auto& guiManager = canvas->GetScene().GetGuiManager();
    auto& layers     = guiManager.GetGuiLayers();

    std::vector<std::string> layerNames;
    for (const auto& l : layers)
    {
        if (l.GetName() == "consoleLayer")
            continue;

        layerNames.push_back(l.GetName());
    }

    AddElementDialog dlg(this, layerNames);
    int result = dlg.ShowModal();

    if (result == wxID_ADD)
    {
        OnCreateNewLayer(event);
        OnAddElement(event);
        return;
    }

    if (result == wxID_OK)
    {
        OnAddElementToLayer(dlg.GetSelectedLayer(), event);
    }
}

void GuiEditorFrame::OnAddElementToLayer(const std::string& layer, wxCommandEvent& event)
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
        canvas->GetScene().GetGuiManager().Add(layer, std::move(newElem));
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
    selectedLayer   = data ? data->layer : nullptr;

    if (selectedElement and data->element)
    {
        propGrid->Append(new wxPropertyCategory("Transform"));
        auto pos   = selectedElement->GetLocalPosition();
        auto scale = selectedElement->GetLocalScale();
        propGrid->Append(new wxFloatProperty("Position X", "PosX", pos.x));
        propGrid->Append(new wxFloatProperty("Position Y", "PosY", pos.y));
        propGrid->Append(new wxFloatProperty("Scale X", "ScaleX", scale.x));
        propGrid->Append(new wxFloatProperty("Scale Y", "ScaleY", scale.y));

        if (auto* txt = dynamic_cast<GameEngine::GuiButtonElement*>(selectedElement))
        {
            propGrid->Append(new wxPropertyCategory("Button Settings"));
            propGrid->Append(new wxStringProperty("Action name", "ActionName", ""));
        }
        if (auto* txt = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
        {
            propGrid->Append(new wxPropertyCategory("Text Settings"));
            propGrid->Append(new wxStringProperty("Label Text", "LabelText", txt->GetText()));
        }
        if (auto* txt = dynamic_cast<GameEngine::GuiTextureElement*>(selectedElement))
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
    }
    if (selectedLayer and data->layer)
    {
        propGrid->Append(new wxPropertyCategory("Layer"));
        propGrid->Append(new wxBoolProperty("IsShow", "LayerIsShow", data->layer->isShow()));
    }
}

void GuiEditorFrame::OnPropertyChange(wxPropertyGridEvent& event)
{
    auto* p   = event.GetProperty();
    auto name = p->GetName();

    if (selectedElement)
    {
        if (name == "PosX")
        {
            vec2 pos = selectedElement->GetLocalPosition();
            pos.x    = p->GetValue().GetDouble();
            selectedElement->SetLocalPosition(pos);
        }
        if (name == "PosY")
        {
            vec2 pos = selectedElement->GetLocalPosition();
            pos.y    = p->GetValue().GetDouble();
            selectedElement->SetLocalPosition(pos);
        }
        if (name == "ScaleX")
        {
            vec2 scale = selectedElement->GetLocalScale();
            scale.x    = p->GetValue().GetDouble();
            selectedElement->SetLocalScale(scale);
        }
        if (name == "ScaleY")
        {
            vec2 scale = selectedElement->GetLocalScale();
            scale.y    = p->GetValue().GetDouble();
            selectedElement->SetLocalScale(scale);
        }
        if (name == "LabelText")
        {
            if (auto text = dynamic_cast<GameEngine::GuiTextElement*>(selectedElement))
            {
                text->SetText(p->GetValue());
            }
        }
    }

    if (selectedLayer)
    {
        if (name == "LayerIsShow")
        {
            p->GetValue().GetBool() ? selectedLayer->Show() : selectedLayer->Hide();
        }
    }

    canvas->Refresh();
}

void GuiEditorFrame::OnOpen(wxCommandEvent&)
{
    wxFileDialog openFileDialog(this, "Choose file", lastDirPath.string(), "", "Gui file (*.xml)|*.xml|All files (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    auto path = openFileDialog.GetPath();
    GameEngine::File file{std::string{path.c_str()}};
    if (not file.exist())
        return;

    GameEngine::GuiElementReader reader(canvas->GetScene().GetGuiManager(), canvas->GetScene().GetGuiElementFactory());
    if (reader.Read(file.GetAbsolutePath()))
    {
        currentFile = file;
        lastDirPath = currentFile->GetAbsolutePath().parent_path();
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
    wxFileDialog fileDialog(this, "Choose file", lastDirPath.string(), "", "Gui file (*.xml)|*.xml|All files (*.*)|*.*",
                            wxFD_SAVE);

    if (fileDialog.ShowModal() == wxID_CANCEL)
        return;

    auto path = fileDialog.GetPath();
    GameEngine::File file{std::string{path.c_str()}};

    if (not file.HasExtension())
    {
        file.ChangeExtension("xml");
    }

    canvas->GetScene().GetGuiManager().SaveToFile(file.GetAbsolutePath());
    currentFile = file;
    lastDirPath = currentFile->GetAbsolutePath().parent_path();
}
void GuiEditorFrame::OnUndo(wxCommandEvent&)
{ /* Logika Undo w silniku */
}
void GuiEditorFrame::OnRedo(wxCommandEvent&)
{ /* Logika Redo w silniku */
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
    auto root    = sceneTree->AddRoot("GUI Root");
    auto& layers = canvas->GetScene().GetGuiManager().GetGuiLayers();
    for (auto& layer : layers)
    {
        if (layer.GetName() == "consoleLayer")
            continue;

        auto layerItemName = "[Layer] " + layer.GetName();
        auto layerId       = sceneTree->AppendItem(root, layerItemName, -1, -1, new GuiTreeItemData(&layer));

        for (auto& element : layer.GetElements())
        {
            AddTreeItem(layerId, element.get());
        }
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
void GuiEditorFrame::OnCreateNewLayer(wxCommandEvent&)
{
    wxTextEntryDialog dlg(this, "Enter new layer name:", "Create Layer", "NewLayer_1");

    if (dlg.ShowModal() == wxID_OK)
    {
        auto layerName = dlg.GetValue().ToStdString();
        canvas->GetScene().GetGuiManager().AddLayer(layerName);
        RefreshTree();
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
        auto* ptrToSelect = newElem.get();

        if (data->element)
        {
            data->element->AddChild(std::move(newElem));
        }
        else if (data->layer)
        {
            canvas->GetScene().GetGuiManager().Add(data->layer->GetName(), std::move(newElem));
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
        canvas->GetScene().GetGuiManager().Remove(*data.element);
        selectedElement = nullptr;
    }
    if (data.layer)
    {
        canvas->GetScene().GetGuiManager().RemoveLayer(data.layer->GetName());
        selectedLayer = nullptr;
    }

    propGrid->Clear();
    RefreshTree();
    canvas->Refresh();
}
