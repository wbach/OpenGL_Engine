#pragma once
#include <wx/process.h>
#include <mutex>

class BuildComponentLogFrame;
class BuildProcess : public wxProcess
{
public:
    BuildProcess(BuildComponentLogFrame*);

    void ReadOutput();
    std::mutex& GetMutex();
    void Stop();

private:
    BuildComponentLogFrame* log;
    std::mutex mutex;
    bool isRunning{true};
};
