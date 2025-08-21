#include "TreeHelper.h"

TreeHelper::TreeHelper(wxTreeCtrl *tree)
    : tree(tree)
{
}

void TreeHelper::DisableItem(const wxTreeItemId &item)
{
    if (!item.IsOk())
        return;
    ApplyRecursive(item, true);
}

void TreeHelper::EnableItem(const wxTreeItemId &item)
{
    if (!item.IsOk())
        return;
    ApplyRecursive(item, false);
}

bool TreeHelper::IsDisabled(const wxTreeItemId &item) const
{
    auto it = disabledMap.find(item);
    if (it != disabledMap.end())
        return it->second;
    return false;
}

bool TreeHelper::CanInteract(const wxTreeItemId &item) const
{
    return !IsDisabled(item);
}

void TreeHelper::RemovePrefabSuffix(wxString &str)
{
    wxString suffix = " (prefab)";
    str.Replace(suffix, "");
}

void TreeHelper::ApplyRecursive(const wxTreeItemId &item, bool disable)
{
    // Zapisz stan
    disabledMap[item] = disable;

    // Zmień wygląd
    if (disable)
    {
        tree->SetItemTextColour(item, *wxLIGHT_GREY);
        tree->SetItemFont(item, tree->GetFont().Italic());
    }
    else
    {
        auto name = tree->GetItemText(item);
        RemovePrefabSuffix(name);
        tree->SetItemText(item, name);

        tree->SetItemTextColour(item, tree->GetForegroundColour());
        tree->SetItemFont(item, tree->GetFont());
    }

    // Rekurencja po dzieciach
    wxTreeItemIdValue cookie;
    wxTreeItemId child = tree->GetFirstChild(item, cookie);
    while (child.IsOk())
    {
        ApplyRecursive(child, disable);
        child = tree->GetNextChild(item, cookie);
    }
}
