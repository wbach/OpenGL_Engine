#pragma once
#include "Types.h"

namespace Utils
{
namespace Thread
{
class Worker;
class ThreadSubscriber;

typedef std::function<void(float deltaTime)> FrameFunc;

class IThreadSync
{
public:
    virtual ~IThreadSync() = default;

    virtual uint32 Subscribe(FrameFunc func, const std::string& label = "unnamed", uint32 = 60) = 0;
    virtual ThreadSubscriber* GetSubscriber(uint32)                                             = 0;
    virtual void Unsubscribe(uint32 id)                                                         = 0;
    virtual void Start()                                                                        = 0;
    virtual void Stop()                                                                         = 0;
    virtual size_t SubscribersCount() const                                                     = 0;
    virtual void setTimeMultiplayer(float)                                                      = 0;

    virtual Worker& AddWorker()        = 0;
    virtual void RemoveWorker(Worker&) = 0;
};
}  // namespace Thread
}  // namespace Utils