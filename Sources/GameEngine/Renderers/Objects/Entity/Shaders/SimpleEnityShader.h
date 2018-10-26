#pragma once
#include "EntityShader.h"

namespace GameEngine
{
class SimpleEntityShader : public EntityShader
{
public:
    SimpleEntityShader(IGraphicsApiPtr graphicsApi);
    virtual void Init() override;
};
}  // namespace GameEngine
