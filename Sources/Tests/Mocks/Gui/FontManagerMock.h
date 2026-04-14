#pragma once
#include <GameEngine/Renderers/GUI/Text/IFontManager.h>
#include <gmock/gmock.h>
#include "Resources/File.h"

namespace GameEngine
{
namespace GUI
{
class FontManagerMock : public IFontManager
{
public:
    MOCK_METHOD(std::optional<IdType>, openFont, (FontStyle, FontSize, FontOutline, const std::optional<FileHandle>&), (override));
    MOCK_METHOD(std::optional<TextureData>, renderText, (const std::string&, IdType, const std::optional<TextWrapWidth>&),
                (override));
    MOCK_METHOD(void, closeFont, (IdType), (override));
};
}  // namespace GUI
}  // namespace GameEngine
