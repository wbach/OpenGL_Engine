#include "TextureButton.h"

#include <GameEngine/Resources/File.h>
#include <wx/sizer.h>

#include <optional>

#include "ProjectManager.h"

namespace
{
static int textureButtonIndex = 0;
}

TextureButton::TextureButton(wxWindow* parent, const std::optional<GameEngine::File>& textureFile, MenuOption option,
                             OnClickFunc onclick, OnChange onchange, OnRemoveFunc onRemoveFunc, const wxSize& size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    , textureFile{textureFile}
    , onclick(onclick)
    , onChange{onchange}
    , onRemove{onRemoveFunc}
    , size{size}
    , index(textureButtonIndex++)
    , menuOption(option)
{
    UpdateBitmap();

    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &TextureButton::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &TextureButton::OnClick, this);
    Bind(wxEVT_ENTER_WINDOW, &TextureButton::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &TextureButton::OnMouseLeave, this);

    if (menuOption != MenuOption::None)
        Bind(wxEVT_RIGHT_DOWN, &TextureButton::OnRightClick, this);
}

void TextureButton::Reset()
{
    textureFile.reset();
    UpdateBitmap();
}

void TextureButton::SetBitmap(const wxBitmap& bmp)
{
    wxImage img = bmp.ConvertToImage();
    img.Rescale(size.x, size.y, wxIMAGE_QUALITY_HIGH);
    bitmap = wxBitmap(img);
    Refresh();
}

void TextureButton::SetBitmap(const GameEngine::File& file)
{
    textureFile = file;
    wxBitmap newBmp(textureFile->GetAbsolutePath().c_str(), wxBITMAP_TYPE_ANY);
    if (not newBmp.IsOk())
    {
        LOG_WARN << "Create texture error : " << textureFile;
        return;
    }
    SetBitmap(newBmp);
    SetToolTip(file.GetBaseName());
}

void TextureButton::UpdateBitmap()
{
    if (textureFile)
    {
        SetBitmap(*textureFile);
    }
    else
    {
        wxBitmap bmp(size);
        wxMemoryDC dc(bmp);
        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
        dc.Clear();
        dc.DrawText("P", size.x / 2 - 5, size.y / 2 - 8);
        dc.SelectObject(wxNullBitmap);
        SetBitmap(bmp);
    }
}
void TextureButton::SetHover(bool hover)
{
    hover = hover;
    Refresh();
}
void TextureButton::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    dc.DrawBitmap(bitmap, 0, 0, true);
    if (ActiveIndex() == index)
    {
        wxPen pen(*wxBLUE, 3);
        dc.SetPen(pen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(1, 1, GetSize().x - 2, GetSize().y - 2);
    }
    else if (hover)
    {
        wxPen pen(*wxLIGHT_GREY, 2);
        dc.SetPen(pen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(1, 1, GetSize().x - 2, GetSize().y - 2);
    }
}
void TextureButton::OnClick(wxMouseEvent&)
{
    ActiveIndex() = index;

    for (auto* child : GetParent()->GetChildren())
    {
        if (auto* btn = dynamic_cast<TextureButton*>(child))
        {
            btn->SetHover(false);
            btn->Refresh();
        }
    }

    Refresh();

    if (onclick and textureFile and textureFile->exist())
        onclick(*textureFile);
}

std::optional<GameEngine::File> TextureButton::SelectFileDialog()
{
    wxFileDialog openFile(this, "Select Texture", ProjectManager::GetInstance().GetDataDir(), "",
                          "Images (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFile.ShowModal() == wxID_OK)
    {
        wxString path = openFile.GetPath();
        SetBitmap(GameEngine::File{path.ToStdString()});
        return path.ToStdString();
    }
    return std::nullopt;
}

void TextureButton::OnRightClick(wxMouseEvent&)
{
    wxMenu menu;
    int ID_CHANGE_TEXTURE = wxWindow::NewControlId();
    menu.Append(ID_CHANGE_TEXTURE, "Change Texture");
    menu.Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [this](wxCommandEvent&)
        {
            auto oldFile = textureFile;

            auto maybeFile = SelectFileDialog();

            if (onChange)
            {
                onChange(oldFile, *maybeFile);
            }
        },
        ID_CHANGE_TEXTURE);

    if (menuOption == MenuOption::ChangeAndRemove)
    {
        int ID_REMOVE = wxWindow::NewControlId();
        menu.Append(ID_REMOVE, "Remove Texture");
        menu.Bind(
            wxEVT_COMMAND_MENU_SELECTED,
            [this](wxCommandEvent&)
            {
                if (onRemove)
                    onRemove();
            },
            ID_REMOVE);
    }

    PopupMenu(&menu);
}
void TextureButton::OnMouseEnter(wxMouseEvent&)
{
    SetHover(true);
}
void TextureButton::OnMouseLeave(wxMouseEvent&)
{
    SetHover(false);
}
