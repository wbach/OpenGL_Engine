#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <map>

class TreeHelper
{
public:
    TreeHelper(wxTreeCtrl* tree) : tree(tree) {}

    void DisableItem(const wxTreeItemId& item)
    {
        if (!item.IsOk()) return;
        ApplyRecursive(item, true);
    }

    void EnableItem(const wxTreeItemId& item)
    {
        if (!item.IsOk()) return;
        ApplyRecursive(item, false);
    }

    bool IsDisabled(const wxTreeItemId& item) const
    {
        auto it = disabledMap.find(item);
        if (it != disabledMap.end())
            return it->second;
        return false;
    }

    bool CanInteract(const wxTreeItemId& item) const
    {
        return !IsDisabled(item);
    }

private:
    wxTreeCtrl* tree;
    std::map<wxTreeItemId, bool> disabledMap;

    void ApplyRecursive(const wxTreeItemId& item, bool disable)
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
};
