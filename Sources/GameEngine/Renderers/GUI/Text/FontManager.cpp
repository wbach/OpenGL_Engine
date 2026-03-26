#include "FontManager.h"

#include <Logger/Log.h>
#include <SDL2/SDL_ttf.h>
#include <Utils/Image/ImageUtils.h>

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
    struct Font
    {
        TTF_Font *ptr{nullptr};
        size_t instances{0};
    };
    std::unordered_map<uint32, Font> fonts_;

    void Clear()
    {
        for (auto &[_, font] : fonts_)
        {
            if (font.ptr)
                TTF_CloseFont(font.ptr);
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
        LOG_ERROR << "Failed to init TTF";
    }
}

FontManager::~FontManager()
{
    LOG_DEBUG << "destructor";
    impl_->Clear();
    TTF_Quit();
}
std::optional<uint32> FontManager::openFont(const File &filename, uint32 size)
{
    if (not isInit_)
        return std::nullopt;

    auto fname = filename.GetAbsolutePath().string() + std::to_string(size);
    auto iter  = fontNameToIdMap_.find(fname);
    if (iter != fontNameToIdMap_.end())
    {
        ++impl_->fonts_[iter->second].instances;
        return iter->second;
    }

    auto percentFontSize = EngineConf.window.size->y * size / 768;
    // auto percentFontSize = windowSize.y / size;

    LOG_DEBUG << "Font percent size : " << percentFontSize << "/" << size;

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
        font = TTF_OpenFont(filename.GetAbsolutePath().string().c_str(), static_cast<int>(percentFontSize));
    }

    if (font)
    {
        // TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
        auto id           = idPool.getId();
        impl_->fonts_[id] = {.ptr = font, .instances = 0};
        fontNameToIdMap_.insert({fname, id});
        return id;
    }

    LOG_ERROR << "Cannot open font : " << filename.GetFilename();
    return {};
}

std::optional<FontManager::TextureData> FontManager::renderFont(uint32 fontId, const std::string &text, uint32 outline,
                                                                uint32 wrapWidth)
{
    if (not isInit_ or text.empty())
        return std::nullopt;

    auto iter = impl_->fonts_.find(fontId);
    if (iter == impl_->fonts_.end())
    {
        return std::nullopt;
    }

    const auto &font = iter->second;

    SDL_Color sdlColor;
    sdlColor.r = 255;
    sdlColor.g = 255;
    sdlColor.b = 255;
    sdlColor.a = 255;

    if (outline > 0)
        TTF_SetFontOutline(font.ptr, static_cast<int>(outline));

    SDL_Surface *sdlSurface = nullptr;
    if (wrapWidth > 0)
    {
        uint32 scaledWrapWidth = (EngineConf.window.size->y * wrapWidth) / 768;
        LOG_DEBUG << "Wrapped width : " << wrapWidth << ", scaledWrapWidth = " << scaledWrapWidth;

        sdlSurface = TTF_RenderText_Blended_Wrapped(font.ptr, text.c_str(), sdlColor, wrapWidth);
    }
    else
    {
        sdlSurface = TTF_RenderText_Blended(font.ptr, text.c_str(), sdlColor);
    }

    if (not sdlSurface)
    {
        LOG_ERROR << "Cannot make a text texture" << SDL_GetError();
        return {};
    }

    SDL_LockSurface(sdlSurface);

    FontManager::TextureData sdlSizeImage;
    sdlSizeImage.name = text + "_" + std::to_string(fontId) + "_" + std::to_string(outline);

    uint32 w   = static_cast<uint32>(sdlSurface->w);
    uint32 h   = static_cast<uint32>(sdlSurface->h);
    uint32 bpp = sdlSurface->format->BytesPerPixel;

    sdlSizeImage.image.setChannels(bpp);
    sdlSizeImage.image.width  = w;
    sdlSizeImage.image.height = h;
    sdlSizeImage.image.pitch  = sdlSurface->pitch;

    size_t totalMemorySize = sdlSurface->pitch * h;
    std::vector<uint8> data(totalMemorySize);
    std::memcpy(data.data(), sdlSurface->pixels, totalMemorySize);
    sdlSizeImage.image.moveData(std::move(data));

    SDL_UnlockSurface(sdlSurface);
    //    Utils::SaveImage(sdlSizeImage.image, text);
    //    SDL_SaveBMP(sdlSurface, (text + ".bmp").c_str());

    SDL_FreeSurface(sdlSurface);
    return sdlSizeImage;
}
void FontManager::closeFont(uint32 fontId)
{
    if (not impl_)
        return;

    auto iter = impl_->fonts_.find(fontId);
    if (iter == impl_->fonts_.end())
    {
        return;
    }

    --iter->second.instances;

    if (iter->second.instances <= 0)
    {
        LOG_DEBUG << "All instances of font are closed, closing the font";
        if (iter->second.ptr)
        {
            TTF_CloseFont(iter->second.ptr);
        }

        impl_->fonts_.erase(iter);

        for (auto fnIter = fontNameToIdMap_.begin(); fnIter != fontNameToIdMap_.end();)
        {
            if (fnIter->second == fontId)
            {
                fnIter = fontNameToIdMap_.erase(fnIter);
            }
            else
            {
                ++fnIter;
            }
        }
    }
}
}  // namespace GameEngine
