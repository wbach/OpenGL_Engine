#pragma once
#include <GameEngine/Resources/File.h>
#include <Utils/Image/Image.h>
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
        Utils::Image image;
    };
    FontManager();
    ~FontManager();

    std::optional<uint32> openFont(const File&, uint32);
    std::optional<TextureData> renderFont(uint32, const std::string&, uint32);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;
    std::unordered_map<std::string, uint32> fontNameToIdMap_;
    bool isInit_;
};
}  // namespace GameEngine
