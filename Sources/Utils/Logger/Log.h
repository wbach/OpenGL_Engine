#pragma once
#include <atomic>
#include <list>
#include <string>

#include "../Types.h"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "Time/TimeMeasurer.h"

enum class LogginLvl
{
    ErrorWarningInfoDebug,
    ErrorWarningInfo,
    ErrorWarning,
    Error,
    None
};

class CLogger
{
public:
    static CLogger& Instance();
    void EnableLogs(LogginLvl = LogginLvl::ErrorWarningInfoDebug);
    void DisableLogs();
    void ImmeditalyLog();
    void LazyLog();
    void InfoLog(const std::string& log);
    void WarningLog(const std::string& log);
    void ErrorLog(const std::string& log);
    void DebugLog(const std::string& log);
    void Logg(const std::string& log);
    void LoggToFileOnly(const std::string& log);
    void SaveToFile() const;
    void AddPrefix(const std::string&);
    void ClearPrefixes();
    ~CLogger();

private:
    CLogger();
    void CreateLogFile();
    void ProccesLog();
    void PrintLogs();
    std::string GetLog();

private:
    LogginLvl logginLvl_;
    std::mutex printMutex_;
    std::string fileName;
    std::list<std::string> logs;
    bool enabled       = false;
    bool logImmeditaly = false;

    std::atomic_bool running_;
    std::thread loggerThread_;
    Utils::Time::CTimeMeasurer timer_;
    std::vector<std::string> prefixes_;
    std::string prefixTotal_;
};
std::string FileNameLogRepresentation(const char*);
// clang-format off
#define LOG_ADD_PREFIX(x) do{CLogger::Instance().AddPrefix(x); } while(0)
#define LOG_CLEAR_PREFIXES() do{CLogger::Instance().ClearPrefixes(); } while(0)
#define LogToFile(x)// do {CLogger::Instance().LoggToFileOnly(x);} while(0)
#define DEBUG_LOG(x) do {CLogger::Instance().DebugLog(FileNameLogRepresentation(__FILE__) + ":" + std::to_string(__LINE__) + " " + __FUNCTION__ + std::string(": ") + x);} while(0)
#define INFO_LOG(x) do {CLogger::Instance().InfoLog(FileNameLogRepresentation(__FILE__) + ":" + std::to_string(__LINE__) + " " + __FUNCTION__ + std::string(": ") + x);} while(0)
#define WARNING_LOG(x) do {CLogger::Instance().WarningLog(FileNameLogRepresentation(__FILE__) + ":" + std::to_string(__LINE__) + " " + __FUNCTION__ + std::string(": ") + x);} while(0)
#define ERROR_LOG(x) do {CLogger::Instance().ErrorLog(FileNameLogRepresentation(__FILE__) + ":" + std::to_string(__LINE__) + " " + __FUNCTION__ + std::string(": ") + x);} while(0)
// clang-format on
