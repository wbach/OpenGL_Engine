#pragma once
#include <gmock/gmock.h>
#include <GameEngine/Renderers/GUI/Text/IFontManager.h>

namespace GameEngine
{
class FontManagerMock : public IFontManager
{
public:
    MOCK_METHOD(std::optional<uint32>, openFont, (const File& file, uint32 fontSize), (override));
    MOCK_METHOD(std::optional<TextureData>, renderFont, (uint32 fontId, const std::string& text, uint32 color), (override));
    MOCK_METHOD(void, closeFont, (uint32 fontId), (override));
};
}  // namespace GameEngine
