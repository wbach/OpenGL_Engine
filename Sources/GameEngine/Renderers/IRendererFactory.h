#pragma once
#include <memory>
#include "GameEngine/Renderers/BaseRenderer.h"

namespace GameEngine
{
class BaseRenderer;
struct RendererContext;

class IRendererFactory
{
public:
    virtual ~IRendererFactory() = default;

    virtual std::unique_ptr<BaseRenderer> create(RendererContext&) const = 0;
};
}  // namespace GameEngine