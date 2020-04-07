#include "FontManager.h"

#include <Logger/Log.h>
#include <SDL2/SDL_ttf.h>

#include <algorithm>

namespace GameEngine
{
struct FontManager::Pimpl
{
    std::vector<TTF_Font*> fonts_;
    std::vector<SDL_Surface*> surfaces_;

    void Clear()
    {
        for (auto& surface : surfaces_)
        {
            DeleteSurface(surface);
        }
        for (auto& font : fonts_)
        {
            TTF_CloseFont(font);
        }
    }
    void DeleteSurface(SDL_Surface*& surface)
    {
        if (surface)
        {
            SDL_FreeSurface(surface);
            surface = nullptr;
        }
    }
};

FontManager::FontManager(const vec2ui& windowSize)
    : windowSize_(windowSize)
{
    impl_ = std::make_unique<FontManager::Pimpl>();

    if (TTF_Init() < 0)
    {
        ERROR_LOG("Failed to init TTF");
    }
}

FontManager::~FontManager()
{
    impl_->Clear();
    TTF_Quit();
}

std::optional<uint32> FontManager::OpenFont(const std::string& filename, uint32 size)
{
    auto fname = filename + std::to_string(size);
    if (fontNameToIdMap_.count(fname) > 0)
    {
        return fontNameToIdMap_.at(fname);
    }

    auto percentFontSize = windowSize_.y * size / 768;
    // auto percentFontSize = windowSize_.y / size;

    DEBUG_LOG("Font percent size : " + std::to_string(percentFontSize) + "/" + std::to_string(size));

    auto font = TTF_OpenFont(filename.c_str(), static_cast<int>(percentFontSize));

    if (font)
    {
        impl_->fonts_.push_back(font);
        auto id = impl_->fonts_.size();
        fontNameToIdMap_.insert({fname, id});
        return id;
    }

    ERROR_LOG("Cannot open font : " + filename);
    return {};
}
std::optional<Surface> FontManager::RenderFont(uint32 fontId, const std::string& text, const vec4& color,
                                               uint32 outline)
{
    auto iter = std::find_if(rendererdTexts_.begin(), rendererdTexts_.end(), [&](const auto& r) {
        return fontId == r.fontId and text == r.text and color == r.color and outline == r.outline;
    });

    if (iter != rendererdTexts_.end())
    {
        ++iter->insances;
        return iter->surface;
    }

    auto index = fontId - 1;
    if (index >= impl_->fonts_.size())
    {
        return {};
    }
    const auto& font = impl_->fonts_[index];

    SDL_Color sdlColor;
    sdlColor.r = static_cast<uint8>(color.x * 255.f);
    sdlColor.g = static_cast<uint8>(color.y * 255.f);
    sdlColor.b = static_cast<uint8>(color.z * 255.f);
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

    std::optional<uint32> surfaceId;

    for (size_t i = 0; i < impl_->surfaces_.size(); ++i)
    {
        if (impl_->surfaces_[i] == nullptr)
        {
            surfaceId           = i;
            impl_->surfaces_[i] = sdlSurface;
        }
    }

    if (not surfaceId)
    {
        impl_->surfaces_.push_back(sdlSurface);
        surfaceId = impl_->surfaces_.size() - 1;
    }

    RenderedText newText;
    newText.color   = color;
    newText.fontId  = fontId;
    newText.outline = outline;
    newText.text    = text;
    newText.surface =
        Surface{*surfaceId, vec2ui(static_cast<uint32>(sdlSurface->w), static_cast<uint32>(sdlSurface->h)),
                sdlSurface->format->BytesPerPixel, sdlSurface->pixels};

    rendererdTexts_.push_back(newText);

    return newText.surface;
}
void FontManager::DeleteSurface(uint32 surfaceId)
{
    if (surfaceId >= impl_->surfaces_.size())
    {
        return;
    }

    auto iter = std::find_if(rendererdTexts_.begin(), rendererdTexts_.end(),
        [&](const auto& r) { return r.surface.id == surfaceId; });

    if (iter != rendererdTexts_.end())
    {
        --iter->insances;
        if (iter->insances == 0)
        {
            impl_->DeleteSurface(impl_->surfaces_[surfaceId]);
            impl_->surfaces_[surfaceId] = nullptr;
        }
    }
}
}  // namespace GameEngine
