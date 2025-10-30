#include "DisableHelper.h"

DisableHelper::DisableHelper(wxTreeCtrl *tree)
    : tree(tree)
{
}

void DisableHelper::DisableItem(const wxTreeItemId &item)
{
    if (!item.IsOk())
        return;
    ApplyRecursive(item, true);
}

void DisableHelper::EnableItem(const wxTreeItemId &item)
{
    if (!item.IsOk())
        return;
    ApplyRecursive(item, false);
}

bool DisableHelper::IsDisabled(const wxTreeItemId &item) const
{
    auto it = disabledMap.find(item);
    if (it != disabledMap.end())
        return it->second;
    return false;
}

bool DisableHelper::CanInteract(const wxTreeItemId &item) const
{
    return !IsDisabled(item);
}

void DisableHelper::RemovePrefabSuffix(wxString &str)
{
    wxString suffix = " (prefab)";
    str.Replace(suffix, "");
}

void DisableHelper::ApplyRecursive(const wxTreeItemId &item, bool disable)
{
    // Zapisz stan
    disabledMap[item] = disable;

    // Zmien wyglad
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
