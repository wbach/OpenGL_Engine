#pragma once
#include <GameEngine/Renderers/GUI/Text/IGuiTextFactory.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>

namespace GameEngine
{
class GuiTextFactoryMock : public IGuiTextFactory
{
public:
    MOCK_METHOD4(CreateImpl, GuiTextElement*(const std::string&, const std::string&, uint32, uint32));

    virtual std::unique_ptr<GuiTextElement> Create(const std::string& label, const std::string& str, uint32 size, uint32 outline)
    {
        return std::unique_ptr<GuiTextElement>(CreateImpl(label, str, size, outline));
    }
};

}  // namespace GameEngine
