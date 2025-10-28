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
    using MessageBox = std::function<void(const std::string&)>;
    Painter(Utils::Thread::IThreadSync&);
    virtual ~Painter();

    void Start();
    void SetNotifyMessageFunc(MessageBox f)
    {
        messageBox = f;
    }

protected:
    virtual void Paint(const DeltaTime&) = 0;
    void StartThread();

protected:
    MessageBox messageBox;

private:
    Utils::Thread::IThreadSync& threadSync_;
    std::optional<IdType> threadSubId;
};
}  // namespace GameEngine
