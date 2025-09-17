#pragma once
#include <memory>

namespace GameEngine
{
class IRenderer;
struct RendererContext;

class IRendererFactory
{
public:
    virtual ~IRendererFactory() = default;

    virtual std::unique_ptr<IRenderer> create(RendererContext&) const = 0;
};
}  // namespace GameEngine