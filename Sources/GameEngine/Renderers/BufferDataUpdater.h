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

struct BufferDataUpdaterSubscriber
{
    uint32 transformSubscribtionId{0};
    std::unique_ptr<IBufferDataUpdaterSubcriber> bufferDataUpdater_{nullptr};
};

typedef std::vector<BufferDataUpdaterSubscriber> BufferDataUpdaterSubcribers;
typedef std::vector<std::pair<uint32, std::unique_ptr<IBufferDataUpdaterEvent>>> BufferDataUpdaterEvents;

class BufferDataUpdater
{
public:
    void Subscribe(GameObject*);
    void UnSubscribe(GameObject*);
    void Update();
    void UnSubscribeAll();
    void ProcessEvents();
    void AddEvent(uint32, std::unique_ptr<IBufferDataUpdaterEvent>);

private:
    BufferDataUpdaterSubcribers subscribers_;
    BufferDataUpdaterEvents events_;
    std::mutex eventMutex_;
    std::mutex subsribtionMutex_;
};

}  // namespace GameEngine
