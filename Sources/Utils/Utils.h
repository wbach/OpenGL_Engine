#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace Utils
{
template <typename T>
std::string ToStr(const T& t)
{
    std::ostringstream os;
    os << t;
    return os.str();
}

std::string MergeString(const std::vector<std::string>&, const std::string& = "");
std::vector<std::string> SplitString(char* s, int size, char split_char);
std::vector<std::string> SplitString(const std::string& s, char split_char);
std::string ReplaceSlash(const std::string& str);
void RemoveSlashes(std::string& str);
std::string RemoveSlashes(const std::string&);
int NextP2(int a);
bool CheckFileExist(const std::filesystem::path&);
bool CheckExtension(const std::string& file, const std::string& ext);
// catch exception and log
float StringToFloat(const std::string& str);
int StringToInt(const std::string& str);
bool StringToBool(const std::string& str);
std::string BoolToString(bool);
std::string toLower(const std::string&);
template <typename TCont, typename Pred>
auto Filter(const TCont& cont, Pred p)
{
    auto out = cont;
    std::erase_if(out, p);
    return out;
}
std::unordered_map<std::string, std::string> parseArguments(int argc, char* argv[]);

template <class Container, class Key>
bool IsKeyExist(const Container& container, const Key& key)
{
    return std::find(container.begin(), container.end(), key) != container.end();
}

template <class Map, class Key>
typename Map::mapped_type* GetValue(Map& map, const Key& key)
{
    auto iter = map.find(key);
    if (iter != map.end())
    {
        return &iter->second;
    }
    return nullptr;
}
template <class Map, class Key>
const typename Map::mapped_type* GetKey(const Map& map, const Key& key)
{
    auto iter = map.find(key);
    if (iter != map.end())
    {
        return &iter->second;
    }
    return nullptr;
}
}  // namespace Utils
