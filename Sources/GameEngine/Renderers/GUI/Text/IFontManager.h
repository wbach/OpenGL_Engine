#pragma once
#include <GameEngine/Resources/File.h>
#include <Types.h>
#include <Utils/Image/Image.h>

#include <optional>

namespace GameEngine
{
class ENGINE_API IFontManager
{
public:
    struct TextureData
    {
        std::string name;
        Utils::Image image;
    };

    virtual ~IFontManager() = default;

    virtual std::optional<uint32> openFont(const File&, uint32)                                   = 0;
    virtual std::optional<TextureData> renderFont(uint32, const std::string&, uint32, uint32 = 0) = 0;
    virtual void closeFont(uint32)                                                                = 0;
};
}  // namespace GameEngine
