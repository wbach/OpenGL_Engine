#pragma once
#include <Types.h>
#include <Utils/BidirectionalUnorderedMap.h>
#include <wx/treectrl.h>

#include <functional>
#include <unordered_map>
#include "DisableHelper.h"
#include "TreeNode.h"

namespace GameEngine
{
class Scene;
struct AddGameObjectNotifEvent;
struct ModifyGameObjectEvent;
struct RemoveGameObjectEvent;
struct ChangeParentEvent;
struct ClearGameObjectsEvent;
}  // namespace GameEngine

struct TreeItemIdHasher
{
    std::size_t operator()(const wxTreeItemId& id) const
    {
        return std::hash<void*>()(id.GetID());
    }
};

struct TreeItemIdEqual
{
    bool operator()(const wxTreeItemId& lhs, const wxTreeItemId& rhs) const
    {
        return lhs == rhs;
    }
};

class SceneTreeCtrl
{
public:
    using ChangeGameObjectParent = std::function<void(IdType, IdType)>;

    SceneTreeCtrl(wxTreeCtrl* tree, ChangeGameObjectParent);

    void CreateRootGameObject();
    void SelectItem(const wxTreeItemId& item, bool select = true);
    void SelectItemWhenGameObjectBecomeAvaiable(IdType, bool select = true);
    wxTreeItemId AppendItemToRoot(const wxString& text, IdType id);
    wxTreeItemId AppendItemToSelection(const wxString& text, IdType id);
    wxTreeItemId AppendItem(const wxTreeItemId& parent, const wxString& text, IdType id);
    wxTreeItemId AppendItem(IdType parent, const wxString& text, IdType id);
    void DeleteAllItems();
    void Delete(const wxTreeItemId& item);
    void SetItemText(const wxTreeItemId& item, const wxString& text);
    wxTreeItemId GetRootItem() const;
    wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    wxTreeCtrl* GetWxTreeCtrl() const;
    wxTreeItemId GetSelection() const;
    std::optional<wxTreeItemId> Get(IdType);
    std::optional<IdType> Get(const wxTreeItemId&);
    size_t GetChildrenCount() const;
    size_t GetChildrenCount(const wxTreeItemId&) const;
    void EditLabel(const wxTreeItemId&);
    wxString GetItemText(const wxTreeItemId&);
    wxTreeItemId GetItemParent(const wxTreeItemId&);
    bool ItemHasChildren(const wxTreeItemId&);
    void MoveTreeNode(const wxTreeItemId& srcItem, const wxTreeItemId& newParent);
    void OnTreeItemRightClick(wxTreeEvent&);
    void OnObjectDrag(wxTreeEvent&);
    void OnObjectEndDrag(wxTreeEvent&);
    void DisableItem(const wxTreeItemId&);
    void RebuildTree(const GameEngine::Scene&);
    void SubscribeForSceneEvent(GameEngine::Scene&);
    void UnSubscribeForSceneEvent();

private:
    void ProcessEvent(const GameEngine::AddGameObjectNotifEvent&);
    void ProcessEvent(const GameEngine::RemoveGameObjectEvent&);
    void ProcessEvent(const GameEngine::ChangeParentEvent&);
    void ProcessEvent(const GameEngine::ClearGameObjectsEvent&);
    void ProcessEvent(const GameEngine::ModifyGameObjectEvent&);

    void UpdateObjectCount();
    void deleteRecursiveFromMap(const wxTreeItemId& item);

    DisableHelper disableHelper;
    ChangeGameObjectParent changeGameObjectParent;
    wxTreeItemId treeRootId;
    wxTreeItemId treeDragItemId;
    wxTreeItemId treeRightClickedItem;
    wxTreeCtrl* gameObjectsView{nullptr};
    BidirectionalUnorderedMap<wxTreeItemId, IdType, TreeItemIdHasher, TreeItemIdEqual> itemIdToObjectId;
    std::optional<IdType> sceneEventSubId;
    GameEngine::Scene* subscribedScene{nullptr};
    std::optional<IdType> selectObjectIdWhenBecomeAvailable;
};
