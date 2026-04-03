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

    using FontSize      = uint32;
    using FontOutline   = uint32;
    using TextWrapWidth = uint32;

    virtual std::optional<IdType> openFont(FontStyle, FontSize, FontOutline, const std::optional<File>& = std::nullopt) = 0;
    virtual std::optional<TextureData> renderText(const std::string&, IdType, const std::optional<TextWrapWidth>& = 0)  = 0;
    virtual void closeFont(IdType)                                                                                      = 0;
};
}  // namespace GameEngine
