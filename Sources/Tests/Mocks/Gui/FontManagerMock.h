#pragma once
#include <GameEngine/Renderers/GUI/Text/IFontManager.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class FontManagerMock : public IFontManager
{
public:
    MOCK_METHOD(std::optional<IdType>, openFont, (FontStyle, FontSize, FontOutline, const std::optional<File>&), (override));
    MOCK_METHOD(std::optional<TextureData>, renderText, (const std::string&, IdType, const std::optional<TextWrapWidth>&),
                (override));
    MOCK_METHOD(void, closeFont, (IdType), (override));
};
}  // namespace GameEngine
