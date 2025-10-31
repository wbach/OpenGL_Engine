#include "Log.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

#include "Utils/FileSystem/FileSystemUtils.hpp"

namespace
{
std::filesystem::path PrepareLoggerPath(const std::filesystem::path& filename)
{
    Utils::CreateDirectories("./Logs");
    return (std::filesystem::absolute("Logs") / filename).lexically_normal();
}
}  // namespace

struct CLogger::Impl
{
    std::ofstream fileStream_;
    std::mutex printMutex_;
    std::thread loggerThread_;

    void ProcessLogs(CLogger* logger)
    {
        while (logger->running_.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            while (!logger->logs.empty())
            {
                std::string log;
                {
                    std::lock_guard<std::mutex> lk(printMutex_);
                    if (!logger->logs.empty())
                    {
                        log = logger->logs.front();
                        logger->logs.pop_front();
                    }
                }
                if (!log.empty())
                {
                    std::cout << log << std::endl;
                    if (fileStream_.is_open())
                    {
                        fileStream_ << log << std::endl;
                        fileStream_.flush();
                    }
                }
            }
        }
    }
};

CLogger& CLogger::Instance()
{
    static CLogger logger;
    return logger;
}

CLogger::CLogger()
    : pImpl(new Impl)
{
}
CLogger::~CLogger()
{
    running_.store(false);
    if (pImpl->loggerThread_.joinable())
        pImpl->loggerThread_.join();
    if (pImpl->fileStream_.is_open())
        pImpl->fileStream_.close();
    delete pImpl;
}

void CLogger::EnableLogs(LoggingLvl lvl, LogMode mode)
{
    logLevel_ = lvl;
    mode_     = mode;

    if (lvl == LoggingLvl::None)
    {
        enabled = false;
        return;
    }

    enabled = true;
    if (mode_ != LogMode::SCREEN_ONLY and not pImpl->fileStream_.is_open())
    {
        if (filePath_.empty())
            filePath_ = "Logs.txt";

        auto absoluteLogPath = PrepareLoggerPath(filePath_);
        std::cout << "absoluteLogPath : " << absoluteLogPath << std::endl;
        pImpl->fileStream_.open(absoluteLogPath);
        if (!pImpl->fileStream_.is_open())
            throw std::runtime_error("Could not open log file");
    }

    if (useAsync_)
        pImpl->loggerThread_ = std::thread(&Impl::ProcessLogs, pImpl, this);
}

void CLogger::DisableLogs()
{
    enabled = false;
    running_.store(false);
    if (pImpl->loggerThread_.joinable())
        pImpl->loggerThread_.join();
}

void CLogger::UseAsyncLogging(bool async)
{
    useAsync_ = async;
}

bool CLogger::ShouldLog(LoggingLvl messageLvl) const
{
    if (!enabled || logLevel_ == LoggingLvl::None)
        return false;
    return static_cast<int>(messageLvl) >= static_cast<int>(logLevel_);
}

void CLogger::InfoLog(const std::string& log)
{
    if (ShouldLog(LoggingLvl::ErrorWarningInfo))
        LogInternal("[INFO] " + log);
}
void CLogger::WarningLog(const std::string& log)
{
    if (ShouldLog(LoggingLvl::ErrorWarning))
        LogInternal("[WARN] " + log);
}
void CLogger::ErrorLog(const std::string& log)
{
    if (ShouldLog(LoggingLvl::Error))
        LogInternal("[ERROR] " + log);
}
void CLogger::DebugLog(const std::string& log)
{
    if (ShouldLog(LoggingLvl::ErrorWarningInfoDebug))
        LogInternal("[DEBUG] " + log);
}

void CLogger::AddPrefix(const std::string& prefix)
{
    prefixes_.push_back("[" + prefix + "]");
    prefixTotal_.clear();
    for (auto& p : prefixes_)
        prefixTotal_ += p;
}
void CLogger::ClearPrefixes()
{
    prefixes_.clear();
    prefixTotal_.clear();
}

void CLogger::SetLogFilename(const std::filesystem::path& filename)
{
    std::lock_guard<std::mutex> lk(pImpl->printMutex_);
    filePath_ = filename;

    if (mode_ == LogMode::SCREEN_ONLY)
        return;

    if (pImpl->fileStream_.is_open())
    {
        pImpl->fileStream_.close();

        auto absoluteLogPath = PrepareLoggerPath(filePath_);
        pImpl->fileStream_.open(absoluteLogPath);
        if (!pImpl->fileStream_.is_open())
            throw std::runtime_error("Log file open error");
    }
}

void CLogger::LogInternal(const std::string& log)
{
    if (!enabled)
        return;

    // timestamp
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[20];
    std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
                  tm.tm_min, tm.tm_sec);

    std::string finalLog =
        "[" + std::string(buf) + "][" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + "] " + log;

    if (!useAsync_)
    {
        std::cout << finalLog << std::endl;
        if (pImpl->fileStream_.is_open())
        {
            pImpl->fileStream_ << finalLog << std::endl;
            pImpl->fileStream_.flush();
        }
    }
    else
    {
        std::lock_guard<std::mutex> lk(pImpl->printMutex_);
        logs.push_back(finalLog);
    }
}

CLogStream::CLogStream(LoggingLvl lvl)
    : lvl_(lvl)
{
    if (!CLogger::Instance().ShouldLog(lvl_))
        active_ = false;
    else
    {
        active_ = true;
    }
}

CLogStream::CLogStream(LoggingLvl lvl, const char* file, int line, const char* func)
    : lvl_(lvl)
{
    if (!CLogger::Instance().ShouldLog(lvl_))
        active_ = false;
    else
    {
        active_ = true;
        ss_ << file << ":" << line << " " << func << ": ";
        ss_ << CLogger::Instance().prefixTotal_;
    }
}

CLogStream::~CLogStream()
{
    if (!active_)
        return;

    std::string log = ss_.str();
    switch (lvl_)
    {
        case LoggingLvl::Error:
            CLogger::Instance().ErrorLog(log);
            break;
        case LoggingLvl::ErrorWarning:
            CLogger::Instance().WarningLog(log);
            break;
        case LoggingLvl::ErrorWarningInfo:
            CLogger::Instance().InfoLog(log);
            break;
        case LoggingLvl::ErrorWarningInfoDebug:
            CLogger::Instance().DebugLog(log);
            break;
        default:
            break;
    }
}