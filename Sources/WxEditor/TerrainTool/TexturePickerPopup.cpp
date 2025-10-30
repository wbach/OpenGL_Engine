#include "TexturePickerPopup.h"

#include <sstream>

#include "TextureButton.h"

TexturePickerPopup::TexturePickerPopup(wxWindow* parent, const std::vector<TexureInfo>& textures, OnSelectFunc onSelect,
                                       OnAddFunc onAdd, OnChangeFunc onChangeFunc, OnRemoveFunc onRemoveFunc)
    : wxPopupTransientWindow(parent, wxBORDER_SIMPLE)
    , selectedTextureFunc(onSelect)
    , addButtonFunc(onAdd)
    , onChangeFunc(onChangeFunc)
    , onRemoveFunc(onRemoveFunc)
{
    wxPanel* panel = new wxPanel(this);

    // ScrolledWindow wypelniajacy panel
    wxScrolledWindow* scrolledWindow = new wxScrolledWindow(panel, wxID_ANY);
    scrolledWindow->SetScrollRate(20, 20);

    // WrapSizer z przyciskami
    wxWrapSizer* wrapSizer = new wxWrapSizer(wxHORIZONTAL);
    createNewTextureButton(scrolledWindow, wrapSizer);
    createTexutreButtons(textures, scrolledWindow, wrapSizer);
    scrolledWindow->SetSizer(wrapSizer);
    wrapSizer->SetSizeHints(scrolledWindow);

    // Sizer panelu - scrolledWindow wypelnia caly panel
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(scrolledWindow, 1, wxEXPAND);
    panel->SetSizer(panelSizer);

    // Sizer popupu
    wxBoxSizer* popupSizer = new wxBoxSizer(wxVERTICAL);
    popupSizer->Add(panel, 1, wxEXPAND);
    SetSizer(popupSizer);

    int numberElementsInRow = 3;  // chcemy 3 przyciski w rzedzie
    int numberRowsVisible   = 2;  // i 2 rzedy widoczne
    int elementWidth        = texSize;
    int elementHeight       = texSize;
    int extraMargin         = 20;

    // Obliczamy dokladna szerokosc i wysokosc popupu
    int popupWidth  = numberElementsInRow * (elementWidth + 2 * spacing) + extraMargin;
    int popupHeight = numberRowsVisible * (elementHeight + 2 * spacing) + extraMargin;

    // Ustawiamy rozmiar popupu
    SetSize(wxSize(popupWidth, popupHeight));
}
void TexturePickerPopup::createNewTextureButton(wxScrolledWindow* scrolledWindow, wxWrapSizer* wrapSizer)
{
    wxButton* btn = new wxButton(scrolledWindow, wxID_ANY, "+", wxDefaultPosition, wxSize(texSize, texSize));
    wrapSizer->Add(btn, 0, wxALL, spacing);

    btn->Bind(wxEVT_BUTTON,
              [this](auto&)
              {
                  Dismiss();
                  addButtonFunc();
              });
}
void TexturePickerPopup::createTexutreButtons(const std::vector<TexureInfo>& textures, wxScrolledWindow* scrolledWindow,
                                              wxWrapSizer* wrapSizer)
{
    for (const auto& textureInfo : textures)
    {
        const auto& textureFile = textureInfo.file;
        if (not textureFile.exist())
        {
            LOG_WARN << "Create texture error, file not exist :" << textureFile;
            continue;
        }

        auto* btn = new TextureButton(
            scrolledWindow, textureFile, TextureButton::MenuOption::ChangeAndRemove,
            [this](const GameEngine::File& f)
            {
                Dismiss();
                if (selectedTextureFunc)
                    selectedTextureFunc(f);
            },
            [this](const auto& oldFile, const auto& newFile)
            {
                Dismiss();
                if (onChangeFunc)
                    onChangeFunc(oldFile, newFile);
            },
            [this, file = textureFile]()
            {
                Dismiss();
                onRemoveFunc(file);
            });

        std::stringstream usedInfo;
        usedInfo << textureFile.GetBaseName();

        if (textureInfo.usedIn.empty())
        {
            usedInfo << "\n\nTexture is not used right now.";
        }
        else
        {
            usedInfo << "\n\nTexture is used in: ";
            for (const auto& info : textureInfo.usedIn)
            {
                usedInfo << "\n" << info;
            }
        }

        btn->SetToolTip(usedInfo.str());

        wrapSizer->Add(btn, 0, wxALL, spacing);
    }
}
