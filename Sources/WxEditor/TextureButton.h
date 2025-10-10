#pragma once
#include <wx/dcbuffer.h>
#include <wx/wx.h>

class TextureButton : public wxPanel
{
public:
    TextureButton(wxWindow* parent, const wxBitmap& bmp, const wxString& name, int index)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, bmp.GetSize())
        , m_bitmap(bmp)
        , m_name(name)
        , m_index(index)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetToolTip(m_name);

        Bind(wxEVT_PAINT, &TextureButton::OnPaint, this);
        Bind(wxEVT_LEFT_DOWN, &TextureButton::OnClick, this);
        Bind(wxEVT_ENTER_WINDOW, &TextureButton::OnMouseEnter, this);
        Bind(wxEVT_LEAVE_WINDOW, &TextureButton::OnMouseLeave, this);
        Bind(wxEVT_RIGHT_DOWN, &TextureButton::OnRightClick, this);
    }

    static int& ActiveIndex()
    {
        static int idx = -1;
        return idx;
    }

    void SetBitmap(const wxBitmap& bmp)
    {
        m_bitmap = bmp;
        Refresh();
    }

    void SetHover(bool hover)
    {
        m_hover = hover;
        Refresh();
    }

private:
    wxBitmap m_bitmap;
    wxString m_name;
    int m_index;
    bool m_hover = false;

    void OnPaint(wxPaintEvent&)
    {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();

        dc.DrawBitmap(m_bitmap, 0, 0, true);
        if (ActiveIndex() == m_index)
        {
            wxPen pen(*wxBLUE, 3);
            dc.SetPen(pen);
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(1, 1, GetSize().x - 2, GetSize().y - 2);
        }
        else if (m_hover)
        {
            wxPen pen(*wxLIGHT_GREY, 2);
            dc.SetPen(pen);
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.DrawRectangle(1, 1, GetSize().x - 2, GetSize().y - 2);
        }
    }

    void OnClick(wxMouseEvent&)
    {
        ActiveIndex() = m_index;

        for (auto* child : GetParent()->GetChildren())
        {
            if (auto* btn = dynamic_cast<TextureButton*>(child))
            {
                btn->SetHover(false);
                btn->Refresh();
            }
        }

        Refresh();
        wxLogMessage("Selected texture %d: %s", m_index + 1, m_name);
    }

    void OnRightClick(wxMouseEvent&)
    {
        wxMenu menu;
        menu.Append(wxID_ANY, "Change Texture");

        // Obsługa wyboru opcji
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED,
                  [this](wxCommandEvent&)
                  {
                      // Otwieramy dialog pliku, aby wybrać nową bitmapę
                      wxFileDialog openFile(this, "Select Texture", "", "", "Images (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp",
                                            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
                      if (openFile.ShowModal() == wxID_OK)
                      {
                          wxString path = openFile.GetPath();
                          wxBitmap newBmp(path, wxBITMAP_TYPE_ANY);
                          if (newBmp.IsOk())
                          {
                              SetBitmap(newBmp);
                              SetToolTip(path);
                              wxLogMessage("Texture changed to: %s", path);
                          }
                      }
                  });

        PopupMenu(&menu);
    }

    void OnMouseEnter(wxMouseEvent&)
    {
        SetHover(true);
    }
    void OnMouseLeave(wxMouseEvent&)
    {
        SetHover(false);
    }
};
