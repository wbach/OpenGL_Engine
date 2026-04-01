#pragma once
#include <GameEngine/Renderers/GUI/Text/IFontManager.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class FontManagerMock : public IFontManager
{
public:
    MOCK_METHOD(std::optional<uint32>, openFont, (const File&, FontStyle, uint32, uint32), (override));
    MOCK_METHOD(std::optional<TextureData>, renderFont, (uint32, const std::string&, uint32), (override));
    MOCK_METHOD(void, closeFont, (uint32), (override));
};
}  // namespace GameEngine
