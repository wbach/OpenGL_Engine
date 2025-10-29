#pragma once
#include <GameEngine/Resources/File.h>
#include <wx/popupwin.h>
#include <wx/sizer.h>
#include <wx/wrapsizer.h>
#include <wx/wx.h>

#include <vector>

class TexturePickerPopup : public wxPopupTransientWindow
{
public:
    using OnSelectFunc = std::function<void(const GameEngine::File&)>;
    using OnRemoveFunc = std::function<void(const GameEngine::File&)>;
    using OnChangeFunc = std::function<void(std::optional<GameEngine::File>, const GameEngine::File&)>;
    using OnAddFunc    = std::function<void()>;

    struct TexureInfo
    {
        GameEngine::File file;
        std::vector<std::string> usedIn;
    };

    TexturePickerPopup(wxWindow*, const std::vector<TexureInfo>&, OnSelectFunc, OnAddFunc, OnChangeFunc, OnRemoveFunc);

private:
    void createNewTextureButton(wxScrolledWindow*, wxWrapSizer*);
    void createTexutreButtons(const std::vector<TexureInfo>&, wxScrolledWindow*, wxWrapSizer*);

private:
    int spacing       = 5;
    const int texSize = 64;
    OnSelectFunc selectedTextureFunc;
    OnAddFunc addButtonFunc;
    OnChangeFunc onChangeFunc;
    OnRemoveFunc onRemoveFunc;
};
