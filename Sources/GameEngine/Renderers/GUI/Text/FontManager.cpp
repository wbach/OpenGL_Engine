#include "FontManager.h"

#include <Logger/Log.h>
#include <SDL2/SDL_ttf.h>
#include <Utils/Image/ImageUtils.h>

#include <algorithm>
#include <filesystem>
#include <optional>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/DefaultFiles/segoe-ui.h"

namespace GameEngine
{
namespace
{
uint32 getPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            return *p;
            break;

        case 2:
            return *(uint16 *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(uint32 *)p;
            break;

        default:
            return 0; /* shouldn't happen, but avoids warnings */
    }
}
}  // namespace

struct FontManager::Pimpl
{
    std::vector<TTF_Font *> fonts_;

    void Clear()
    {
        for (auto &font : fonts_)
        {
            TTF_CloseFont(font);
        }
    }
};

FontManager::FontManager()
    : isInit_{false}
{
    impl_   = std::make_unique<FontManager::Pimpl>();
    isInit_ = TTF_Init() >= 0;

    if (not isInit_)
    {
        ERROR_LOG("Failed to init TTF");
    }
}

FontManager::~FontManager()
{
    impl_->Clear();
    TTF_Quit();
}
std::optional<uint32> FontManager::openFont(const File &filename, uint32 size)
{
    if (not isInit_)
        return std::nullopt;

    auto fname = filename.GetAbsolutePath() + std::to_string(size);
    if (fontNameToIdMap_.count(fname) > 0)
    {
        return fontNameToIdMap_.at(fname);
    }

    auto percentFontSize = EngineConf.window.size->y * size / 768;
    // auto percentFontSize = windowSize.y / size;

    DEBUG_LOG("Font percent size : " + std::to_string(percentFontSize) + "/" + std::to_string(size));

    TTF_Font *font{nullptr};

    if (not std::filesystem::exists(filename.GetAbsolutePath().c_str()))
    {
        SDL_RWops *rw = SDL_RWFromConstMem(segoe_ui_ttf, segoe_ui_ttf_len);
        if (!rw)
        {
            LOG_ERROR << "Create default font error: " << SDL_GetError();
            return std::nullopt;
        }

        font = TTF_OpenFontRW(rw, 1, 24);  // '1' = SDL zwolni pamięć RWops automatycznie
        if (!font)
        {
            LOG_ERROR << "Create default font error: " << TTF_GetError();
            return std::nullopt;
        }
    }
    else
    {
        font = TTF_OpenFont(filename.GetAbsolutePath().c_str(), static_cast<int>(percentFontSize));
    }

    if (font)
    {
        // TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
        impl_->fonts_.push_back(font);
        auto id = impl_->fonts_.size();
        fontNameToIdMap_.insert({fname, id});
        return id;
    }

    ERROR_LOG("Cannot open font : " + filename.GetFilename());
    return {};
}

std::optional<FontManager::TextureData> FontManager::renderFont(uint32 fontId, const std::string &text, uint32 outline)
{
    if (not isInit_ or text.empty())
        return std::nullopt;

    auto index = fontId - 1;
    if (index >= impl_->fonts_.size())
    {
        return std::nullopt;
    }

    const auto &font = impl_->fonts_[index];

    SDL_Color sdlColor;
    sdlColor.r = 255;
    sdlColor.g = 255;
    sdlColor.b = 255;
    sdlColor.a = 255;

    if (outline > 0)
        TTF_SetFontOutline(font, static_cast<int>(outline));

    auto sdlSurface = TTF_RenderText_Blended(font, text.c_str(), sdlColor);

    if (not sdlSurface)
    {
        ERROR_LOG("Cannot make a text texture" + std::string(SDL_GetError()));
        return {};
    }

    SDL_LockSurface(sdlSurface);

    FontManager::TextureData sdlSizeImage;
    sdlSizeImage.name = text + "_" + std::to_string(fontId) + "_" + std::to_string(outline);
    sdlSizeImage.image.setChannels(sdlSurface->format->BytesPerPixel);
    sdlSizeImage.image.width  = static_cast<uint32>(sdlSurface->w);
    sdlSizeImage.image.height = static_cast<uint32>(sdlSurface->h);
    // sdlSizeImage.image.copyImage<uint8>(sdlSurface->pixels);
    sdlSizeImage.image.allocateImage<uint8>();

    for (uint32 y = 0; y < static_cast<uint32>(sdlSurface->h); y++)
    {
        for (uint32 x = 0; x < static_cast<uint32>(sdlSurface->w); x++)
        {
            SDL_Color color;
            Uint32 data = getPixel(sdlSurface, x, y);
            SDL_GetRGBA(data, sdlSurface->format, &color.r, &color.g, &color.b, &color.a);
            sdlSizeImage.image.setPixel({x, y}, Color(vec4ui8(color.r, color.g, color.b, color.a)));
        }
    }
    SDL_UnlockSurface(sdlSurface);
    //    Utils::SaveImage(sdlSizeImage.image, text);
    //    SDL_SaveBMP(sdlSurface, (text + ".bmp").c_str());

    SDL_FreeSurface(sdlSurface);
    return sdlSizeImage;
}
}  // namespace GameEngine
