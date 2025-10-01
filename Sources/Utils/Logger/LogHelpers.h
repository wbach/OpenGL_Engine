#pragma once
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <deque>
#include <set>
#include <unordered_set>
#include <vector>
#include <list>
#include <type_traits>
#include <iostream>

// ------------------ Trait: czy typ można wypisać ------------------
template <typename T, typename = void>
struct is_streamable : std::false_type {};

template <typename T>
struct is_streamable<T, decltype(void(std::declval<std::ostream&>() << std::declval<T>()))>
    : std::true_type {};

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
template <typename T, typename... Args> struct is_sequence<std::deque<T,Args...>>  : std::true_type {};
template <typename T> inline constexpr bool is_sequence_v = is_sequence<T>::value;

// ------------------ Tryb wypisywania ------------------
enum class PrintMode { Full, KeysOnly, ValuesOnly };
inline PrintMode g_printMode = PrintMode::Full;

inline std::ostream& KEYS_ONLY(std::ostream& os)   { g_printMode = PrintMode::KeysOnly; return os; }
inline std::ostream& VALUES_ONLY(std::ostream& os) { g_printMode = PrintMode::ValuesOnly; return os; }
inline std::ostream& FULL(std::ostream& os)        { g_printMode = PrintMode::Full; return os; }

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
    g_printMode = PrintMode::Full; // reset trybu
    return os;
}

// ------------------ formatString ------------------
template <typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

inline std::string formatString(const std::string& fmt)
{
    if (fmt.find("{}") != std::string::npos)
        throw std::runtime_error("empty string.");
    return fmt;
}

template <typename T, typename... Args>
std::string formatString(const std::string& fmt, T&& first, Args&&... rest)
{
    size_t pos = fmt.find("{}");
    if (pos == std::string::npos)
        throw std::runtime_error("to many arguments");

    std::string result    = fmt.substr(0, pos) + toString(std::forward<T>(first));
    std::string remaining = fmt.substr(pos + 2);

    if constexpr (sizeof...(rest) > 0)
        result += formatString(remaining, std::forward<Args>(rest)...);
    else {
        if (remaining.find("{}") != std::string::npos)
            throw std::runtime_error("not enough arguments");
        result += remaining;
    }

    return result;
}
