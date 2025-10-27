#pragma once
#include <GameEngine/Resources/File.h>
#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include <wx/wx.h>

#include <functional>
#include <optional>

class TextureButton : public wxPanel
{
public:
    using OnClickFunc  = std::function<void(const GameEngine::File&)>;
    using OnRemoveFunc = std::function<void()>;

    TextureButton(wxWindow* parent, const std::optional<GameEngine::File>&, bool, OnClickFunc onclick, OnRemoveFunc onRemoveFunc,
                  const wxSize& = wxSize(64, 64));

    void SetBitmap(const GameEngine::File&);
    void SetHover(bool hover);
    void UpdateBitmap();
    std::optional<GameEngine::File> SelectFileDialog();
    void Reset();

private:
    void SetBitmap(const wxBitmap& bmp);
    void OnPaint(wxPaintEvent&);
    void OnClick(wxMouseEvent&);
    void OnRightClick(wxMouseEvent&);
    void OnMouseEnter(wxMouseEvent&);
    void OnMouseLeave(wxMouseEvent&);

    static int& ActiveIndex()
    {
        static int idx = -1;
        return idx;
    }

private:
    std::optional<GameEngine::File> textureFile;
    OnClickFunc m_onclick;
    OnRemoveFunc onRemove;
    wxSize size;
    wxBitmap m_bitmap;
    wxString m_name;
    int m_index;
    bool m_hover = false;
    bool hasMenu{false};
};
