#pragma once
#include <Mutex.hpp>
#include <memory>
#include "GameEngine/Renderers/IRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IBufferDataUpdaterEvent.h"
#include "IBufferDataUpdaterSubcriber.h"

namespace GameEngine
{
namespace Components
{
class RendererComponent;
}  // namespace Components

typedef std::vector<std::unique_ptr<IBufferDataUpdaterSubcriber>> BufferDataUpdaterSubcribers;
typedef std::vector<std::unique_ptr<IBufferDataUpdaterEvent>> BufferDataUpdaterEvents;

class BufferDataUpdater
{
public:
    BufferDataUpdater(GraphicsApi::IGraphicsApi& graphicsApi);
    ~BufferDataUpdater();

    void Subscribe(GameObject* gameObject);
    void UnSubscribe(GameObject* gameObject);
    void Update();
    void UnSubscribeAll();
    void ProcessEvents();
    void AddEvent(std::unique_ptr<IBufferDataUpdaterEvent>);

private:
    BufferDataUpdaterSubcribers subscribers_;
    BufferDataUpdaterEvents events_;
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::mutex eventMutex_;
};

}  // namespace GameEngine
