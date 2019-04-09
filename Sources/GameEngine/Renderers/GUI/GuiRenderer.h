#pragma once
#include <memory>
#include <vector>
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererFunctionType.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GuiElement.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IGuiRenderer.h"
#include "Logger/Log.h"

namespace GameEngine
{
struct Time;
class GUIRenderer : public IRenderer
{
public:
    GUIRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory);
    virtual ~GUIRenderer() override;
    virtual void Init() override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    void Render(const Scene& scene, const Time&);

    template <class T>
    T* Get(const std::string& name);
    void Add(const std::string& name, std::unique_ptr<GuiElement> element);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::vector<std::unique_ptr<IGuiRenderer>> renderers_;
    std::unordered_map<std::string, GuiElement*> elementsMap_;
    std::vector<std::unique_ptr<GuiElement>> elements_;
};
template <class T>
T* GUIRenderer::Get(const std::string& name)
{
    if (elementsMap_.count(name) > 0)
    {
        auto& element = elementsMap_.at(name);

        if (element->GetType() == T::type)
        {
            return static_cast<T*>(element);
        }
        else
        {
            Error("Can not get " + std::to_string(static_cast<int>(T::type)) +
                ", because element is type of :" + std::to_string(static_cast<int>(element->GetType())));
        }
    }
    else
    {
        Log("Element with name : " + name + " not found.");
    }
    return nullptr;
}
}  // namespace GameEngine
