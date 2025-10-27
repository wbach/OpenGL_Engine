#pragma once
#include <vector>
#include <GameEngine/Resources/File.h>
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/popupwin.h>
#include <wx/wrapsizer.h>

class TexturePickerPopup : public wxPopupTransientWindow
{
public:
    using OnSelectFunc = std::function<void(const GameEngine::File&)>;
    using OnRemoveFunc = std::function<void(const GameEngine::File&)>;
    using OnAddFunc    = std::function<void()>;

    TexturePickerPopup(wxWindow* parent, std::vector<GameEngine::File>& textures, OnSelectFunc onSelect, OnAddFunc onAdd,
                       OnRemoveFunc onRemoveFunc);

private:
    void createNewTextureButton(wxScrolledWindow*, wxWrapSizer*);
    void createTexutreButtons(wxScrolledWindow*, wxWrapSizer*);

private:
    std::vector<GameEngine::File>& textures;
    int spacing       = 5;
    const int texSize = 64;
    OnSelectFunc selectedTextureFunc;
    OnAddFunc addButtonFunc;
    OnRemoveFunc onRemoveFunc;
};