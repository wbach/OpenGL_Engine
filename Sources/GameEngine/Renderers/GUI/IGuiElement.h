#pragma once

namespace GameEngine
{
class IGuiElement
{
public:
    virtual ~IGuiElement()
    {
    }
    virtual void Init()           = 0;
    virtual void Render()         = 0;
    virtual void UnSubscribeAll() = 0;
    virtual void ReloadShaders()  = 0;
};
}  // namespace GameEngine
