#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include <memory>

namespace GameEngine
{
namespace Components
{
class RendererComponent;
}  // namespace Components


class IBufferDataUpdaterSubcriber
{
public:
    virtual ~IBufferDataUpdaterSubcriber() {}
    virtual void Update() = 0;
    virtual uint32 GetId() const = 0;
};

typedef std::vector<std::unique_ptr<IBufferDataUpdaterSubcriber>> BufferDataUpdaterSubcribers;

class BufferDataUpdater
{
public:
    BufferDataUpdater(GraphicsApi::IGraphicsApi& graphicsApi);
    ~BufferDataUpdater();

    void Subscribe(GameObject* gameObject);
    void UnSubscribe(GameObject* gameObject);
    void Update();
    void UnSubscribeAll();

private:
    BufferDataUpdaterSubcribers subscribers_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
};

}  // namespace GameEngine
