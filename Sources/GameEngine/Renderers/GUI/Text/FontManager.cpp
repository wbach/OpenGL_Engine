#include "FontManager.h"

#include <Logger/Log.h>
#include <SDL2/SDL_ttf.h>

#include <algorithm>

#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
struct FontManager::Pimpl
{
    std::vector<TTF_Font*> fonts_;

    void Clear()
    {
        for (auto& font : fonts_)
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
std::optional<uint32> FontManager::openFont(const File& filename, uint32 size)
{
    if (not isInit_)
        return std::nullopt;

    auto fname = filename.GetAbsoultePath() + std::to_string(size);
    if (fontNameToIdMap_.count(fname) > 0)
    {
        return fontNameToIdMap_.at(fname);
    }

    auto percentFontSize = EngineConf.window.size->y * size / 768;
    // auto percentFontSize = windowSize.y / size;

    DEBUG_LOG("Font percent size : " + std::to_string(percentFontSize) + "/" + std::to_string(size));

    auto font = TTF_OpenFont(filename.GetAbsoultePath().c_str(), static_cast<int>(percentFontSize));

    if (font)
    {
        impl_->fonts_.push_back(font);
        auto id = impl_->fonts_.size();
        fontNameToIdMap_.insert({fname, id});
        return id;
    }

    ERROR_LOG("Cannot open font : " + filename.GetFilename());
    return {};
}

std::optional<FontManager::TextureData> FontManager::renderFont(uint32 fontId, const std::string& text, uint32 outline)
{
    if (not isInit_ or text.empty())
        return std::nullopt;

    auto index = fontId - 1;
    if (index >= impl_->fonts_.size())
    {
        return std::nullopt;
    }

    const auto& font = impl_->fonts_[index];

    SDL_Color sdlColor;
    sdlColor.r = 255;
    sdlColor.g = 255;
    sdlColor.b = 255;
    sdlColor.a = 255;

    if (outline > 0)
        TTF_SetFontOutline(font, static_cast<int>(outline));
    auto sdlSurface = TTF_RenderText_Blended(font, text.c_str(), sdlColor);
    if (outline > 0)
        TTF_SetFontOutline(font, static_cast<int>(outline));
    if (not sdlSurface)
    {
        ERROR_LOG("Cannot make a text texture" + std::string(SDL_GetError()));
        return {};
    }

    FontManager::TextureData sdlSizeImage;
    sdlSizeImage.name = text + "_" + std::to_string(fontId) + "_" + std::to_string(outline);
    sdlSizeImage.image.setChannels(sdlSurface->format->BytesPerPixel);
    sdlSizeImage.image.width  = static_cast<uint32>(sdlSurface->w);
    sdlSizeImage.image.height = static_cast<uint32>(sdlSurface->h);
    sdlSizeImage.image.copyImage<uint8>(sdlSurface->pixels);

    SDL_FreeSurface(sdlSurface);
    return sdlSizeImage;
}
}  // namespace GameEngine
