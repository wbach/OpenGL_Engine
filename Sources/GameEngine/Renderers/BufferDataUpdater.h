#pragma once
#include <Mutex.hpp>
#include <memory>
#include <unordered_map>

#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/TransformDataEvent.h"
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
    GameObject* gameObject{nullptr};
};

typedef std::vector<BufferDataUpdaterSubscriber> BufferDataUpdaterSubcribers;
typedef std::unordered_map<uint32, TransformDataEvent> BufferDataUpdaterEvents;

class BufferDataUpdater
{
public:
    void Subscribe(GameObject*);
    void UnSubscribe(GameObject*);
    void Update();
    void UnSubscribeAll();
    void ProcessEvents();
    void AddEvent(uint32, TransformDataEvent&&);

private:
    BufferDataUpdaterSubcribers subscribers_;
    BufferDataUpdaterEvents events_;
    std::mutex eventMutex_;
    std::mutex subsribtionMutex_;
};

}  // namespace GameEngine
