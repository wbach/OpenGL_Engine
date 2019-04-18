#pragma once
#include <Types.h>
#include <memory>

namespace GameEngine
{
class GuiTextElement;

class IGuiTextFactory
{
public:
    virtual std::unique_ptr<GuiTextElement> Create(const std::string&, const std::string&, uint32, uint32) =0;
    virtual ~IGuiTextFactory() = default;
};
}  // namespace GameEngine
