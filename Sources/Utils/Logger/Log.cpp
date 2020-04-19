#include "Log.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

#include "SDL2/SDL_messagebox.h"

CLogger& CLogger::Instance()
{
    static CLogger logger;
    return logger;
}

void CLogger::EnableLogs()
{
    enabled = true;
    CreateLogFile();
    if (not logImmeditaly)
        loggerThread_ = std::thread(std::bind(&CLogger::ProccesLog, this));
}
void CLogger::ImmeditalyLog()
{
    logImmeditaly = true;
}
void CLogger::LazyLog()
{
    std::lock_guard<std::mutex> lk(printMutex_);
    logImmeditaly = false;
}
void CLogger::ErrorLog(const std::string& log)
{
    Logg("[ERROR] " + prefixTotal_ + log);
}

void CLogger::DebugLog(const std::string& log)
{
    Logg("[DEBUG] " + prefixTotal_ + log);
}
void CLogger::Logg(const std::string& log)
{
    if (not enabled)
        return;

    std::lock_guard<std::mutex> lk(printMutex_);

    std::stringstream ss;
    auto now            = std::chrono::system_clock::now();
    std::time_t time    = std::chrono::system_clock::to_time_t(now);
    std::string timeStr = std::ctime(&time);
    timeStr.pop_back();

    ss << "[" << timeStr << "][" << std::this_thread::get_id() << "]" << log;

    if (logImmeditaly)
    {
        std::cout << ss.str() << std::endl;

        std::ofstream file(fileName, std::ios_base::app);
        file << ss.str() << std::endl;
        file.close();
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
void CLogger::SaveToFile() const
{
    std::ofstream file(fileName, std::ios_base::app);
    for (const auto& log : logs)
        file << log.c_str() << '\n';
    file.close();
}

void CLogger::AddPrefix(const std::string& prefix)
{
    prefixes_.push_back("[" + prefix + "]");
    prefixTotal_.clear();

    for (const auto& p : prefixes_)
    {
        prefixTotal_ += p;
    }
}

void CLogger::ClearPrefixes()
{
    prefixes_.clear();
    prefixTotal_.clear();
}
CLogger::~CLogger()
{
    if (enabled)
    {
        running_.store(false);
        loggerThread_.join();
        PrintLogs();
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

    std::filesystem::create_directories("./Logs");
    fileName = "Logs/Logs.txt";
    std::ofstream file(fileName);
    file.close();
}

void CLogger::ProccesLog()
{
    while (running_.load())
    {
        timer_.StartFrame();
        PrintLogs();
        timer_.EndFrame();
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
