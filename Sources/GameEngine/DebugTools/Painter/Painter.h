#pragma once
#include <Types.h>

#include <string>

namespace Utils::Thread
{
class IThreadSync;
}

namespace GameEngine
{
static const std::string PAINTER_THREAD_NAME{"PainterUpdate"};

class Painter
{
public:
    Painter(Utils::Thread::IThreadSync&);
    virtual ~Painter();

    void Start();

protected:
    virtual void Paint(const DeltaTime&) = 0;
    void StartThread();

private:
    Utils::Thread::IThreadSync& threadSync_;
    std::optional<IdType> threadSubId;
};
}  // namespace GameEngine
