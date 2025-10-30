#pragma once
#include <wx/treectrl.h>
#include <wx/wx.h>

#include <map>

class DisableHelper
{
public:
    DisableHelper(wxTreeCtrl*);

    void DisableItem(const wxTreeItemId&);
    void EnableItem(const wxTreeItemId&);
    bool IsDisabled(const wxTreeItemId&) const;
    bool CanInteract(const wxTreeItemId&) const;

private:
    wxTreeCtrl* tree;
    std::map<wxTreeItemId, bool> disabledMap;

    void RemovePrefabSuffix(wxString&);
    void ApplyRecursive(const wxTreeItemId&, bool disable);
};
