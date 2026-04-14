#include "FontManager.h"

#include <Logger/Log.h>
#include <SDL2/SDL_ttf.h>
#include <Utils/Image/ImageUtils.h>
#include <Variant.h>

#include <filesystem>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <unordered_map>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Resources/DefaultFiles/segoe-ui.h"
#include "GameEngine/Resources/MemoryData.h"
namespace GameEngine
{
namespace GUI
{
namespace
{
const std::unordered_map<std::string, MemoryData> memoryFonts{
    {"segoe-ui.ttf", MemoryData{.data = segoe_ui_ttf, .length = segoe_ui_ttf_len}}};

int ToSDLStyle(FontStyle style)
{
    auto sdlStyle = TTF_STYLE_NORMAL;
    auto s        = static_cast<uint32>(style);

    if (s & static_cast<uint32>(FontStyle::Bold))
        sdlStyle |= TTF_STYLE_BOLD;
    if (s & static_cast<uint32>(FontStyle::Italic))
        sdlStyle |= TTF_STYLE_ITALIC;
    if (s & static_cast<uint32>(FontStyle::Underline))
        sdlStyle |= TTF_STYLE_UNDERLINE;
    if (s & static_cast<uint32>(FontStyle::Strikethrough))
        sdlStyle |= TTF_STYLE_STRIKETHROUGH;

    return sdlStyle;
}

auto loadFontFromMemory = [](const auto &data, const auto dataLength, int ptSize) -> TTF_Font *
{
    auto *rw = SDL_RWFromConstMem(data, dataLength);
    if (not rw)
    {
        LOG_ERROR << "Create default font error: " << SDL_GetError();
        return nullptr;
    }

    auto font = TTF_OpenFontRW(rw, 1, ptSize);
    if (not font)
    {
        LOG_ERROR << "Create default font error: " << TTF_GetError();
    }
    return font;
};
}  // namespace
struct FontManager::Pimpl
{
    struct Font
    {
        std::string name;
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
std::optional<IdType> FontManager::openFont(FontStyle style, FontSize size, FontOutline outline,
                                            const std::optional<FileHandle> &fileHandle)
{
    if (not isInit_)
        return std::nullopt;

    auto fname = getFontName(fileHandle, style, size, outline);

    auto iter = fontNameToIdMap_.find(fname);
    if (iter != fontNameToIdMap_.end())
    {
        ++impl_->fonts_[iter->second].instances;
        return iter->second;
    }

    auto percentFontSize = size;//static_cast<int>(EngineConf.window.size->y * size / 768);
    // auto percentFontSize = windowSize.y / size;

    LOG_DEBUG << "Font size : " << percentFontSize << " (actual skaled) /" << size << " (requested)";

    TTF_Font *font{nullptr};
    if (fileHandle)
    {
        std::visit(visitor{[&](const MemoryFile &m)
                           {
                               auto iter = memoryFonts.find(m.name);
                               if (iter != memoryFonts.end())
                               {
                                   auto data = iter->second;
                                   font      = loadFontFromMemory(data.data, data.length, percentFontSize);

                                   if (not font)
                                   {
                                       LOG_ERROR << "Create font error: " << TTF_GetError();
                                   }
                               }
                               else
                               {
                                   LOG_ERROR << "Font not found in memory : " << m;
                               }
                           },
                           [&](const File &file)
                           { font = TTF_OpenFont(file.GetAbsolutePath().string().c_str(), percentFontSize); }},
                   *fileHandle);
    }
    else
    {
        font = loadFontFromMemory(segoe_ui_ttf, segoe_ui_ttf_len, percentFontSize);
        if (not font)
        {
            LOG_ERROR << "Create default font error: " << TTF_GetError();
            return std::nullopt;
        }
    }

    if (font)
    {
        if (outline > 0)
        {
            TTF_SetFontOutline(font, static_cast<int>(outline));
        }

        TTF_SetFontStyle(font, ToSDLStyle(style));
        auto id           = idPool.getId();
        impl_->fonts_[id] = {.name = fname, .ptr = font, .instances = 1};
        fontNameToIdMap_.insert({fname, id});
        return id;
    }

    LOG_ERROR << "Cannot open font : " << fileHandle;
    return {};
}

std::optional<TextureData> FontManager::renderText(const std::string &text, IdType fontId,
                                                   const std::optional<TextWrapWidth> &wrapWidth)
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

    SDL_Surface *sdlSurface = nullptr;
    if (wrapWidth and wrapWidth > 0)
    {
        uint32 scaledWrapWidth = (EngineConf.window.size->y * *wrapWidth) / 768;
        LOG_DEBUG << "text:" << text << ", Wrapped width : " << wrapWidth << ", scaledWrapWidth = " << scaledWrapWidth;

        sdlSurface = TTF_RenderText_Blended_Wrapped(font.ptr, text.c_str(), sdlColor, scaledWrapWidth);
    }
    else
    {
        LOG_DEBUG << "Render text: " << text;
        sdlSurface = TTF_RenderText_Blended(font.ptr, text.c_str(), sdlColor);
    }

    if (not sdlSurface)
    {
        LOG_ERROR << "Cannot make a text texture" << SDL_GetError();
        return {};
    }

    SDL_LockSurface(sdlSurface);

    TextureData sdlSizeImage;
    sdlSizeImage.name = text + "_" + font.name;

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

    LOG_DEBUG << "Created image: " << sdlSizeImage.image;

    SDL_UnlockSurface(sdlSurface);
    //    Utils::SaveImage(sdlSizeImage.image, text);
    //    SDL_SaveBMP(sdlSurface, (text + ".bmp").c_str());

    SDL_FreeSurface(sdlSurface);
    return sdlSizeImage;
}
void FontManager::closeFont(IdType fontId)
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
std::string FontManager::getFontName(const std::optional<FileHandle> &fileHandle, FontStyle style, uint32 size,
                                     uint32 outline) const
{
    std::string path{"defaultFont"};

    if (fileHandle)
    {
        path = std::visit(
            visitor{[](const MemoryFile &m) { return m.name; }, [](const File &file) { return file.GetAbsolutePath().string(); }},
            *fileHandle);
    }

    return +"_" + std::string(magic_enum::enum_name(style)) + "_" + std::to_string(size) + "_" + std::to_string(outline);
}
}  // namespace GUI
}  // namespace GameEngine
