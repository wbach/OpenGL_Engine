#include "Painter.h"
#include <Logger/Log.h>
#include <Utils/ThreadSync.h>

namespace GameEngine
{
Painter::Painter(Utils::Thread::IThreadSync& threadSync)
    : threadSync_{threadSync}
{
}
void Painter::Start()
{
    StartThread();
    LOG_DEBUG << "Painter enabled";
}

Painter::~Painter()
{
    if (threadSubId)
        threadSync_.Unsubscribe(*threadSubId);
    LOG_DEBUG << "Painter disabled";
}
void Painter::StartThread()
{
    threadSubId = threadSync_.Subscribe([&](float deltaTime) { Paint(deltaTime); }, PAINTER_THREAD_NAME, 30);
}
}  // namespace GameEngine
