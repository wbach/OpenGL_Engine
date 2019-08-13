#include "Log.h"
#include <boost/filesystem.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include "SDL2/SDL_messagebox.h"

namespace
{
std::ofstream mainfile;
}
CLogger& CLogger::Instance()
{
    static CLogger logger;
    return logger;
}

void CLogger::EnableLogs()
{
    enabled = true;
    CreateLogFile();
    loggerThread_ = std::thread(std::bind(&CLogger::ProccesLog, this));
}
void CLogger::ImmeditalyLog()
{
    std::lock_guard<std::mutex> lk(printMutex_);
    logImmeditaly = true;
    mainfile      = std::ofstream(fileName, std::ios_base::app);
}
void CLogger::LazyLog()
{
    std::lock_guard<std::mutex> lk(printMutex_);
    logImmeditaly = false;
    if (mainfile.is_open())
        mainfile.close();
}
void CLogger::ErrorLog(const std::string& log)
{
    MessageBox(SDL_MESSAGEBOX_ERROR, "Error", log.c_str());
    Logg("[ERROR] "+ log);
}

void CLogger::DebugLog(const std::string &log)
{
    Logg("[DEBUG] "+ log);
}
void CLogger::Logg(const std::string& log)
{
    if (not enabled)
        return;

    std::lock_guard<std::mutex> lk(printMutex_);

    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::string timeStr = std::ctime(&time);
    timeStr.pop_back();

    ss << "[" << timeStr << "][" << std::this_thread::get_id() << "]" << log;

    if (logImmeditaly)
    {
        std::cout << ss.str() << std::endl;
        mainfile << ss.str() << std::endl;
    }
    else
    {
        logs.push_back(ss.str());
    }
}
void CLogger::LoggToFileOnly(const std::string& log)
{
    std::lock_guard<std::mutex> lk(printMutex_);

    std::ofstream file(fileName, std::ios_base::app);
    file << "[" << std::this_thread::get_id() << "]" << log << std::endl;
    file.close();
}
void CLogger::MessageBox(uint32 flags, const std::string& title, const std::string& message)
{
    // SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), NULL);
}
void CLogger::SaveToFile() const
{
    std::ofstream file(fileName, std::ios_base::app);
    for (const auto& log : logs)
        file << log.c_str() << '\n';
    file.close();
}
CLogger::~CLogger()
{
    if (enabled)
    {
        running_.store(false);
        loggerThread_.join();
        PrintLogs();

        if (mainfile.is_open())
        {
            mainfile.close();
        }
    }
}

CLogger::CLogger()
    : running_(true)
    , timer_(5)
{
}

void CLogger::CreateLogFile()
{
    auto now       = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << in_time_t;

    boost::filesystem::create_directories("./Logs");
    fileName = "Logs/Logs.txt";
    std::ofstream file(fileName);
    file.close();
}

void CLogger::ProccesLog()
{
    while (running_.load())
    {
        PrintLogs();
        timer_.CalculateAndLock();
    }
}

void CLogger::PrintLogs()
{
    std::ofstream file(fileName, std::ios_base::app);
    while (true)
    {
        auto log = GetLog();
        if (log.empty())
            break;
        std::cout << log << std::endl;
        file << log << '\n';
    }
    file.close();
}

std::string CLogger::GetLog()
{
    std::lock_guard<std::mutex> lk(printMutex_);

    if (logs.empty())
        return "";

    auto l = logs.front();
    logs.pop_front();
    return l;
}
