#pragma once

#include <atomic>
#include <deque>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include "LogHelpers.h"
#include "Utils/StreamOperators.h"
#include "LoggingLvl.h"

class CLogger
{
public:
    static CLogger& Instance();

    enum class LogMode
    {
        BOTH,
        FILE_ONLY,
        SCREEN_ONLY
    };

    void EnableLogs(LoggingLvl lvl = LoggingLvl::ErrorWarningInfoDebug, LogMode = LogMode::BOTH);
    void DisableLogs();
    void SetLogFilename(const std::filesystem::path& filename);

    void UseAsyncLogging(bool async);  // nowa nazwa flagi

    void InfoLog(const std::string& log);
    void WarningLog(const std::string& log);
    void ErrorLog(const std::string& log);
    void DebugLog(const std::string& log);

    void AddPrefix(const std::string& prefix);
    void ClearPrefixes();

    bool ShouldLog(LoggingLvl lvl) const;

    ~CLogger();

private:
    CLogger();
    CLogger(const CLogger&)            = delete;
    CLogger& operator=(const CLogger&) = delete;

    void LogInternal(const std::string& log);

private:
    LoggingLvl logLevel_{LoggingLvl::ErrorWarningInfoDebug};
    bool enabled{false};
    bool useAsync_{false};

    std::deque<std::string> logs;
    std::vector<std::string> prefixes_;
    std::string prefixTotal_;
    std::filesystem::path filePath_;

    struct Impl;
    Impl* pImpl{nullptr};

    std::atomic<bool> running_{true};
    LogMode mode_;

    friend class CLogStream;
};

// ============================================================
// CLogStream minimalny w headerze
class CLogStream
{
public:
    CLogStream(LoggingLvl lvl, const char* file, int line, const char* func);
    ~CLogStream();

    template <typename T>
    CLogStream& operator<<(const T& value)
    {
        if (active_)
            ss_ << value;
        return *this;
    }

private:
    std::ostringstream ss_;
    LoggingLvl lvl_;
    bool active_{true};
};

// ============================================================
// Makra do logowania
#define LOG_INFO CLogStream(LoggingLvl::ErrorWarningInfo, __FILE__, __LINE__, __FUNCTION__)
#define LOG_WARN CLogStream(LoggingLvl::ErrorWarning, __FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR CLogStream(LoggingLvl::Error, __FILE__, __LINE__, __FUNCTION__)
#define LOG_DEBUG CLogStream(LoggingLvl::ErrorWarningInfoDebug, __FILE__, __LINE__, __FUNCTION__)
