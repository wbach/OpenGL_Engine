#include "TexturePickerPopup.h"

#include "TextureButton.h"

TexturePickerPopup::TexturePickerPopup(wxWindow* parent, std::vector<GameEngine::File>& textures, OnSelectFunc onSelect,
                                       OnAddFunc onAdd, OnRemoveFunc onRemoveFunc)
    : wxPopupTransientWindow(parent, wxBORDER_SIMPLE)
    , textures{textures}
    , selectedTextureFunc(onSelect)
    , addButtonFunc(onAdd)
    , onRemoveFunc(onRemoveFunc)
{
    wxPanel* panel = new wxPanel(this);

    // ScrolledWindow wypelniajacy panel
    wxScrolledWindow* scrolledWindow = new wxScrolledWindow(panel, wxID_ANY);
    scrolledWindow->SetScrollRate(20, 20);

    // WrapSizer z przyciskami
    wxWrapSizer* wrapSizer = new wxWrapSizer(wxHORIZONTAL);
    createNewTextureButton(scrolledWindow, wrapSizer);
    createTexutreButtons(scrolledWindow, wrapSizer);
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
void TexturePickerPopup::createTexutreButtons(wxScrolledWindow* scrolledWindow, wxWrapSizer* wrapSizer)
{
    for (const auto& textureFile : textures)
    {
        if (not textureFile.exist())
        {
            LOG_WARN << "Create texture error, file not exist :" << textureFile;
            continue;
        }

        auto* btn = new TextureButton(
            scrolledWindow, textureFile, true,
            [this](const GameEngine::File& f)
            {
                Dismiss();
                if (selectedTextureFunc)
                    selectedTextureFunc(f);
            },
            [this, file = textureFile]()
            {
                auto iter = std::find(textures.begin(), textures.end(), file);
                if (iter != textures.end())
                {
                    textures.erase(iter);
                }
                else
                {
                    LOG_DEBUG << "texture not found " << file;
                }
                Dismiss();

                onRemoveFunc(file);
            });

        wrapSizer->Add(btn, 0, wxALL, spacing);
    }
}
