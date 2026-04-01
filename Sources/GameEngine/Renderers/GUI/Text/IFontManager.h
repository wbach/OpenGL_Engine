#pragma once
#include <GameEngine/Resources/File.h>
#include <Types.h>
#include <Utils/Image/Image.h>

#include <optional>

namespace GameEngine
{
enum class FontStyle : uint32
{
    Normal        = 0x00,
    Bold          = 0x01,
    Italic        = 0x02,
    Underline     = 0x04,
    Strikethrough = 0x08
};
class ENGINE_API IFontManager
{
public:
    struct TextureData
    {
        std::string name;
        Utils::Image image;
    };

    virtual ~IFontManager() = default;

    virtual std::optional<uint32> openFont(const File&, FontStyle, uint32, uint32)        = 0;
    virtual std::optional<TextureData> renderFont(uint32, const std::string&, uint32 = 0) = 0;
    virtual void closeFont(uint32)                                                        = 0;
};
}  // namespace GameEngine
