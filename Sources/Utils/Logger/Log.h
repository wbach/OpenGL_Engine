#pragma once
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

#include <atomic>
#include <cstdlib>
#include <list>
#include <string>

#include "../Types.h"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "Time/TimeMeasurer.h"
#include <sstream>

enum class LogginLvl
{
    ErrorWarningInfoDebug,
    ErrorWarningInfo,
    ErrorWarning,
    Error,
    None
};

class CLogStream;
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

    friend class CLogStream;
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

template <typename T>
std::string typeName()
{
#ifdef _MSC_VER
    return typeid(T).name();
#else
    int status;
    std::string tname    = typeid(T).name();
    char* demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
    if (status == 0)
    {
        tname = demangled_name;
        std::free(demangled_name);
    }
    return tname;
#endif
}

template <typename T>
std::string typeName(const T&)
{
    return typeName<T>();
}

class CLogStream
{
public:
    CLogStream(LogginLvl lvl, const char* file, int line, const char* func)
        : lvl_(lvl)
    {
        // Dodajemy prefix automatyczny: plik:linia funkcja
        ss_ << FileNameLogRepresentation(file) << ":" << line << " " << func << ": ";
        // Dodajemy globalny prefix z loggera
        ss_ << CLogger::Instance().prefixTotal_;
    }

    ~CLogStream()
    {
        // Przekazujemy zawartość strumienia do loggera
        std::string log = ss_.str();
        switch (lvl_)
        {
        case LogginLvl::Error:       CLogger::Instance().ErrorLog(log); break;
        case LogginLvl::ErrorWarning:CLogger::Instance().WarningLog(log); break;
        case LogginLvl::ErrorWarningInfo:
        case LogginLvl::ErrorWarningInfoDebug:
        default:                     CLogger::Instance().InfoLog(log); break;
        }
    }

    template <typename T>
    CLogStream& operator<<(const T& value)
    {
        ss_ << value;
        return *this;
    }

private:
    std::ostringstream ss_;
    LogginLvl lvl_;
};

#define LOG_INFO  CLogStream(LogginLvl::ErrorWarningInfo, __FILE__, __LINE__, __FUNCTION__)
#define LOG_WARN  CLogStream(LogginLvl::ErrorWarning, __FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR CLogStream(LogginLvl::Error, __FILE__, __LINE__, __FUNCTION__)
#define LOG_DEBUG CLogStream(LogginLvl::ErrorWarningInfoDebug, __FILE__, __LINE__, __FUNCTION__)

// konwersja dowolnego typu na string
template <typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// wersja bazowa: brak argumentów
inline std::string formatString(const std::string& fmt)
{
    if (fmt.find("{}") != std::string::npos)
        throw std::runtime_error("Brak argumentów do zastąpienia w '{}' w stringu.");
    return fmt;
}

// wersja rekurencyjna: co najmniej jeden argument
template<typename T, typename... Args>
std::string formatString(const std::string& fmt, T&& first, Args&&... rest)
{
    size_t pos = fmt.find("{}");
    if (pos == std::string::npos)
        throw std::runtime_error("Za dużo argumentów do formatowania!");

    std::string result = fmt.substr(0, pos) + toString(std::forward<T>(first));
    std::string remaining = fmt.substr(pos + 2);

    if constexpr (sizeof...(rest) > 0)
    {
        result += formatString(remaining, std::forward<Args>(rest)...);
    }
    else
    {
        if (remaining.find("{}") != std::string::npos)
            throw std::runtime_error("Za mało argumentów do formatowania!");
        result += remaining;
    }

    return result;
}
