#pragma once
#include <Types.h>
#include <Utils/Image/Image.h>

#include <optional>

#include "FontStyle.h"
#include "GameEngine/Resources/FileHandle.h"
#include "TextureData.h"

namespace GameEngine
{
namespace GUI
{
using FontSize      = uint32;
using FontOutline   = uint32;
using TextWrapWidth = uint32;

class ENGINE_API IFontManager
{
public:
    virtual ~IFontManager() = default;

    virtual std::optional<IdType> openFont(FontStyle, FontSize, FontOutline, const std::optional<FileHandle>& = std::nullopt) = 0;
    virtual std::optional<TextureData> renderText(const std::string&, IdType, const std::optional<TextWrapWidth>& = 0)        = 0;
    virtual void closeFont(IdType)                                                                                            = 0;
};
}  // namespace GUI
}  // namespace GameEngine
