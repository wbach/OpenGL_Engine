#include "SceneTreeCtrl.h"

#include <Logger/Log.h>
#include <Utils/Variant.h>
#include <wx/menu.h>

#include <GameEngine/Scene/Scene.hpp>
#include <memory>

#include "DisableHelper.h"
#include "WxEditor/Commands/AddObjectCommand.h"
#include "WxEditor/Commands/RemoveObjectCommand.h"
#include "WxEditor/Commands/UndoManager.h"
#include "WxEditor/ControlsIds.h"
#include "WxEditor/WxHelpers/EditorUitls.h"

SceneTreeCtrl::SceneTreeCtrl(wxTreeCtrl *tree, ChangeGameObjectParent changeGameObjectParent)
    : disableHelper(tree)
    , changeGameObjectParent(changeGameObjectParent)
    , gameObjectsView(tree)
{
    CreateRootGameObject();
    tree->Bind(wxEVT_TREE_BEGIN_DRAG, &SceneTreeCtrl::OnObjectDrag, this);
    tree->Bind(wxEVT_TREE_END_DRAG, &SceneTreeCtrl::OnObjectEndDrag, this);
    tree->Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &SceneTreeCtrl::OnTreeItemRightClick, this);
}

void SceneTreeCtrl::CreateRootGameObject()
{
    treeRootId = gameObjectsView->AddRoot("Scene (Objects: 0)", 0);
    itemIdToObjectId.Insert(treeRootId, 0);
}

void SceneTreeCtrl::SelectItem(const wxTreeItemId &item, bool select)
{
    gameObjectsView->SelectItem(item, select);
}

void SceneTreeCtrl::SelectItemWhenGameObjectBecomeAvaiable(IdType id, bool select)
{
    if (auto item = Get(id))
    {
        SelectItem(*item, select);
        return;
    }

    selectObjectIdWhenBecomeAvailable = id;
}

wxTreeItemId SceneTreeCtrl::AppendItemToRoot(const wxString &text, IdType id)
{
    return AppendItem(treeRootId, text, id);
}

wxTreeItemId SceneTreeCtrl::AppendItemToSelection(const wxString &text, IdType id)
{
    return AppendItem(GetSelection(), text, id);
}

wxTreeItemId SceneTreeCtrl::AppendItem(const wxTreeItemId &parent, const wxString &text, IdType id)
{
    wxTreeItemId item = gameObjectsView->AppendItem(parent, text);
    itemIdToObjectId.Insert(item, id);
    return item;
}

wxTreeItemId SceneTreeCtrl::AppendItem(IdType parent, const wxString &text, IdType id)
{
    if (auto wxParentId = Get(parent))
    {
        return AppendItem(*wxParentId, text, id);
    }
    return {};
}

void SceneTreeCtrl::DeleteAllItems()
{
    LOG_DEBUG << "";
    gameObjectsView->DeleteAllItems();
    itemIdToObjectId.Clear();
    CreateRootGameObject();
}

void SceneTreeCtrl::Delete(const wxTreeItemId &item)
{
    if (not item.IsOk())
        return;

    if (item == treeRootId)
        return;

    deleteRecursiveFromMap(item);

    if (gameObjectsView)
        gameObjectsView->Delete(item);
}

void SceneTreeCtrl::SetItemText(const wxTreeItemId &item, const wxString &text)
{
    if (gameObjectsView && item.IsOk())
        gameObjectsView->SetItemText(item, text);
}

wxTreeItemId SceneTreeCtrl::GetRootItem() const
{
    return treeRootId;
}

wxTreeItemId SceneTreeCtrl::GetFirstChild(const wxTreeItemId &item, wxTreeItemIdValue &cookie) const
{
    return gameObjectsView ? gameObjectsView->GetFirstChild(item, cookie) : wxTreeItemId();
}

wxTreeItemId SceneTreeCtrl::GetNextChild(const wxTreeItemId &item, wxTreeItemIdValue &cookie) const
{
    return gameObjectsView ? gameObjectsView->GetNextChild(item, cookie) : wxTreeItemId();
}

wxTreeCtrl *SceneTreeCtrl::GetWxTreeCtrl() const
{
    return gameObjectsView;
}

wxTreeItemId SceneTreeCtrl::GetSelection() const
{
    return gameObjectsView->GetSelection();
}

std::optional<wxTreeItemId> SceneTreeCtrl::Get(IdType v)
{
    if (auto id = itemIdToObjectId.Get(v))
    {
        return *id;
    }
    return std::nullopt;
}

std::optional<IdType> SceneTreeCtrl::Get(const wxTreeItemId &v)
{
    if (auto id = itemIdToObjectId.Get(v))
    {
        return *id;
    }
    return std::nullopt;
}

size_t SceneTreeCtrl::GetChildrenCount() const
{
    return gameObjectsView->GetChildrenCount(treeRootId);
}

size_t SceneTreeCtrl::GetChildrenCount(const wxTreeItemId &item) const
{
    return gameObjectsView->GetChildrenCount(item);
}

void SceneTreeCtrl::OnTreeItemRightClick(wxTreeEvent &event)
{
    wxTreeItemId itemId = event.GetItem();
    if (!itemId.IsOk())
        return;

    treeRightClickedItem = event.GetItem();
    gameObjectsView->SelectItem(itemId);

    wxMenu menu;
    menu.Append(ID_TREE_MENU_CREATE_CHILD, "Create child");
    menu.Append(ID_TREE_MENU_CLONE, "Clone");
    menu.AppendSeparator();
    menu.Append(ID_TREE_MENU_MAKE_PREFAB, "Create prefab");
    menu.Append(ID_TREE_MENU_UNMARK_PREFAB, "Unmark prefab");
    menu.AppendSeparator();
    menu.Append(ID_TREE_MENU_RENAME, "Rename");
    menu.AppendSeparator();
    menu.Append(ID_TREE_MENU_REMOVE, "Remove");

    if (disableHelper.IsDisabled(itemId))
    {
        menu.Enable(ID_TREE_MENU_CREATE_CHILD, false);
        menu.Enable(ID_TREE_MENU_MAKE_PREFAB, false);
        menu.Enable(ID_TREE_MENU_RENAME, false);
        menu.Enable(ID_TREE_MENU_CLONE, false);

        wxTreeItemId parent = gameObjectsView->GetItemParent(itemId);
        if (parent.IsOk())
        {
            if (disableHelper.IsDisabled(parent))
            {
                menu.Enable(ID_TREE_MENU_UNMARK_PREFAB, false);
                menu.Enable(ID_TREE_MENU_REMOVE, false);
            }
        }
    }
    gameObjectsView->PopupMenu(&menu);
}

void SceneTreeCtrl::OnObjectDrag(wxTreeEvent &event)
{
    if (not event.GetItem().IsOk())
        return;

    treeDragItemId = event.GetItem();
    event.Allow();
}

void SceneTreeCtrl::OnObjectEndDrag(wxTreeEvent &event)
{
    auto target = event.GetItem();
    if (not target.IsOk() or not treeDragItemId.IsOk())
        return;

    if (target == treeDragItemId)
        return;

    auto maybeDragedGoId = Get(treeDragItemId);
    auto maybeTargetGoId = Get(target);
    if (not maybeDragedGoId or not maybeTargetGoId)
    {
        LOG_WARN << "Draged object id or terget object id not found, move error";
        return;
    }
    changeGameObjectParent(*maybeDragedGoId, *maybeTargetGoId);
    MoveTreeNode(treeDragItemId, target);
    treeDragItemId = {};
}

void SceneTreeCtrl::DisableItem(const wxTreeItemId &item)
{
    disableHelper.DisableItem(item);
}

void SceneTreeCtrl::EditLabel(const wxTreeItemId &item)
{
    gameObjectsView->EditLabel(item);
}

wxString SceneTreeCtrl::GetItemText(const wxTreeItemId &item)
{
    return gameObjectsView->GetItemText(item);
}

wxTreeItemId SceneTreeCtrl::GetItemParent(const wxTreeItemId &item)
{
    return gameObjectsView->GetItemParent(item);
}

bool SceneTreeCtrl::ItemHasChildren(const wxTreeItemId &item)
{
    return gameObjectsView->ItemHasChildren(item);
}

void SceneTreeCtrl::MoveTreeNode(const wxTreeItemId &srcItem, const wxTreeItemId &newParent)
{
    auto maybeGoId = Get(srcItem);

    if (not maybeGoId)
    {
        LOG_ERROR << "Move get gameObjct id error";
        return;
    }

    auto text    = gameObjectsView->GetItemText(srcItem);
    auto *data   = gameObjectsView->GetItemData(srcItem);
    auto newItem = AppendItem(newParent, text, *maybeGoId);
    gameObjectsView->SetItemData(newItem, data);

    wxTreeItemIdValue cookie;
    for (auto child = gameObjectsView->GetFirstChild(srcItem, cookie); child.IsOk();
         child      = gameObjectsView->GetNextChild(srcItem, cookie))
    {
        MoveTreeNode(child, newItem);
    }

    Delete(srcItem);
}

void SceneTreeCtrl::deleteRecursiveFromMap(const wxTreeItemId &item)
{
    wxTreeItemIdValue cookie;
    for (wxTreeItemId child = gameObjectsView->GetFirstChild(item, cookie); child.IsOk();
         child              = gameObjectsView->GetNextChild(item, cookie))
    {
        deleteRecursiveFromMap(child);
    }
    itemIdToObjectId.Erase(item);
}

void SceneTreeCtrl::RebuildTree(const GameEngine::Scene &scene)
{
    DeleteAllItems();

    auto appendChilds = [&](auto &&self, const auto &itemId, const auto &go) -> void
    {
        auto item = AppendItem(itemId, go->GetName(), go->GetId());
        for (const auto &child : go->GetChildren())
        {
            self(self, item, child);
        }
    };

    for (auto &go : scene.GetGameObjects())
    {
        auto item = AppendItem(gameObjectsView->GetRootItem(), go->GetName(), go->GetId());
        for (const auto &child : go->GetChildren())
        {
            appendChilds(appendChilds, item, child);
        }
    }

    UpdateObjectCount();
}

void SceneTreeCtrl::SubscribeForSceneEvent(GameEngine::Scene &scene)
{
    if (sceneEventSubId)
    {
        LOG_WARN << "Previous scene wasn not unsubscrbed!";
    }

    sceneEventSubId = scene.SubscribeForSceneEvent(
        [this](const auto &event)
        {
            std::visit(visitor{[this](const auto &e)
                               {
                                   // call on wxWdidgets thread
                                   gameObjectsView->CallAfter([this, e] { ProcessEvent(e); });
                               }},
                       event);
        });

    subscribedScene = &scene;
}

void SceneTreeCtrl::UnSubscribeForSceneEvent()
{
    if (sceneEventSubId)
    {
        if (not subscribedScene)
        {
            LOG_ERROR << "Something goes wrong, scene not set.";
            return;
        }

        subscribedScene->UnSubscribeForSceneEvent(*sceneEventSubId);
        sceneEventSubId.reset();
        subscribedScene = nullptr;
    }
}

void SceneTreeCtrl::ProcessEvent(const GameEngine::AddGameObjectNotifEvent &event)
{
    LOG_DEBUG << "AddGameObjectNotifEvent process event";
    auto name = event.gameObject->GetName();

    if (isGameObjectPrefab(*event.gameObject))
    {
        name += " (prefab)";
    }

    auto item = AppendItem(event.parentGameObject, name, event.gameObject->GetId());
    UpdateObjectCount();

    if (selectObjectIdWhenBecomeAvailable and event.gameObject->GetId() == *selectObjectIdWhenBecomeAvailable)
    {
        SelectItem(item);
    }


}
void SceneTreeCtrl::ProcessEvent(const GameEngine::RemoveGameObjectEvent &event)
{
    if (auto item = Get(event.gameObjectId))
    {
        Delete(*item);
        UpdateObjectCount();
    }
}
void SceneTreeCtrl::ProcessEvent(const GameEngine::ChangeParentEvent &event)
{
    if (auto item = Get(event.gameObjectId))
    {
        if (auto newParentItem = Get(event.newParentId))
        {
            MoveTreeNode(*item, *newParentItem);
        }
    }
}
void SceneTreeCtrl::ProcessEvent(const GameEngine::ClearGameObjectsEvent &)
{
    DeleteAllItems();
}
void SceneTreeCtrl::ProcessEvent(const GameEngine::ModifyGameObjectEvent &)
{
}

void SceneTreeCtrl::UpdateObjectCount()
{
    auto objectCount = GetChildrenCount();
    SetItemText(treeRootId, "Scene (Objects: " + std::to_string(objectCount) + ")");
}