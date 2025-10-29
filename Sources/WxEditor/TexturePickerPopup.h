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

    struct TexureInfo
    {
        GameEngine::File file;
        std::vector<std::string> usedIn;
    };

    TexturePickerPopup(wxWindow* parent, const std::vector<TexureInfo>& textures, OnSelectFunc onSelect, OnAddFunc onAdd,
                       OnRemoveFunc onRemoveFunc);

private:
    void createNewTextureButton(wxScrolledWindow*, wxWrapSizer*);
    void createTexutreButtons(const std::vector<TexureInfo>&, wxScrolledWindow*, wxWrapSizer*);

private:
    int spacing       = 5;
    const int texSize = 64;
    OnSelectFunc selectedTextureFunc;
    OnAddFunc addButtonFunc;
    OnRemoveFunc onRemoveFunc;
};