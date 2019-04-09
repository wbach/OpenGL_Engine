#pragma once

namespace GameEngine
{
class GuiElement;

class IGuiRenderer
{
public:
    virtual ~IGuiRenderer()             = default;
    virtual void Init()                 = 0;
    virtual void Subscribe(GuiElement*) = 0;
    virtual void Render()               = 0;
    virtual void UnSubscribeAll()       = 0;
    virtual void ReloadShaders()        = 0;
};
}  // namespace GameEngine
