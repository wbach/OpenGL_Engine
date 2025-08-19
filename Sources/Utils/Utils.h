#pragma once
#include <functional>
#include <sstream>
#include <string>
#include <vector>


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
bool CheckFileExist(const std::string& file);
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
}  // namespace Utils
