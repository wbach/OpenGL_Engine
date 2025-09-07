#pragma once
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

#include <atomic>
#include <cstdlib>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <set>
#include <unordered_set>

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
            case LogginLvl::Error:
                CLogger::Instance().ErrorLog(log);
                break;
            case LogginLvl::ErrorWarning:
                CLogger::Instance().WarningLog(log);
                break;
            case LogginLvl::ErrorWarningInfo:
            case LogginLvl::ErrorWarningInfoDebug:
            default:
                CLogger::Instance().InfoLog(log);
                break;
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

#define LOG_INFO CLogStream(LogginLvl::ErrorWarningInfo, __FILE__, __LINE__, __FUNCTION__)
#define LOG_WARN CLogStream(LogginLvl::ErrorWarning, __FILE__, __LINE__, __FUNCTION__)
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
template <typename T, typename... Args>
std::string formatString(const std::string& fmt, T&& first, Args&&... rest)
{
    size_t pos = fmt.find("{}");
    if (pos == std::string::npos)
        throw std::runtime_error("Za dużo argumentów do formatowania!");

    std::string result    = fmt.substr(0, pos) + toString(std::forward<T>(first));
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
// ------------------ Tryb wypisywania ------------------
enum class PrintMode { Full, KeysOnly, ValuesOnly };
inline PrintMode g_printMode = PrintMode::Full;

inline std::ostream& KEYS_ONLY(std::ostream& os)   { g_printMode = PrintMode::KeysOnly; return os; }
inline std::ostream& VALUES_ONLY(std::ostream& os) { g_printMode = PrintMode::ValuesOnly; return os; }
inline std::ostream& FULL(std::ostream& os)        { g_printMode = PrintMode::Full; return os; }

// ------------------ Trait: czy typ można wypisać ------------------
template <typename T, typename = void>
struct is_streamable : std::false_type {};

template <typename T>
struct is_streamable<T, decltype(void(std::declval<std::ostream&>() << std::declval<T>()))> : std::true_type {};

template <typename T>
inline constexpr bool is_streamable_v = is_streamable<T>::value;

// ------------------ Trait: map ------------------
template <typename T> struct is_map : std::false_type {};
template <typename K, typename V, typename... Args> struct is_map<std::map<K,V,Args...>> : std::true_type {};
template <typename K, typename V, typename... Args> struct is_map<std::unordered_map<K,V,Args...>> : std::true_type {};
template <typename T> inline constexpr bool is_map_v = is_map<T>::value;

// ------------------ Trait: set ------------------
template <typename T> struct is_set : std::false_type {};
template <typename K, typename... Args> struct is_set<std::set<K,Args...>> : std::true_type {};
template <typename K, typename... Args> struct is_set<std::unordered_set<K,Args...>> : std::true_type {};
template <typename T> inline constexpr bool is_set_v = is_set<T>::value;

// ------------------ Trait: vector/list ------------------
template <typename T> struct is_sequence : std::false_type {};
template <typename T, typename... Args> struct is_sequence<std::vector<T,Args...>> : std::true_type {};
template <typename T, typename... Args> struct is_sequence<std::list<T,Args...>> : std::true_type {};
template <typename T> inline constexpr bool is_sequence_v = is_sequence<T>::value;

// ------------------ Uniwersalny operator<< dla kontenerów ------------------
template <typename Container,
          typename = std::enable_if_t<is_map_v<Container> || is_set_v<Container> || is_sequence_v<Container>>>
inline std::ostream& operator<<(std::ostream& os, const Container& c)
{
    if constexpr (is_map_v<Container>)
    {
        os << "{";
        bool first = true;
        for (const auto& [key, value] : c)
        {
            if (!first) os << ", ";
            if (g_printMode == PrintMode::KeysOnly) { os << key; }
            else if (g_printMode == PrintMode::ValuesOnly) {
                if constexpr (is_streamable_v<decltype(value)>) os << value;
                else os << "<non-printable>";
            }
            else { // Full
                os << key;
                if constexpr (is_streamable_v<decltype(value)>) os << ": " << value;
                else os << ": <non-printable>";
            }
            first = false;
        }
        os << "}";
    }
    else if constexpr (is_set_v<Container>)
    {
        os << "{";
        bool first = true;
        for (const auto& v : c)
        {
            if (!first) os << ", ";
            if constexpr (is_streamable_v<decltype(v)>) os << v;
            else os << "<non-printable>";
            first = false;
        }
        os << "}";
    }
    else if constexpr (is_sequence_v<Container>)
    {
        os << "[";
        bool first = true;
        for (const auto& v : c)
        {
            if (!first) os << ", ";
            if constexpr (is_streamable_v<decltype(v)>) os << v;
            else os << "<non-printable>";
            first = false;
        }
        os << "]";
    }
    else
    {
        static_assert(is_map_v<Container> || is_set_v<Container> || is_sequence_v<Container>,
                      "operator<< only supports map/set/vector/list types");
    }

    g_printMode = PrintMode::Full; // reset trybu
    return os;
}