#pragma once
#include <GameEngine/Resources/File.h>
#include <GraphicsApi/Image.h>
#include <Types.h>

#include <memory>
#include <optional>
#include <unordered_map>

#include "RenderedText.h"
#include "Surface.h"

namespace GameEngine
{
class FontManager
{
public:
    struct TextureData
    {
        std::string name;
        GraphicsApi::Image image;
    };
    FontManager(const WindowSize&);
    ~FontManager();

    std::optional<uint32> openFont(const File&, uint32 size);
    std::optional<TextureData> renderFont(uint32 id, const std::string& text, uint32 outline);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
    std::unordered_map<std::string, uint32> fontNameToIdMap_;
    const WindowSize& windowSize_;
};
}  // namespace GameEngine